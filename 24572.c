int ha_maria::repair(THD *thd, HA_CHECK *param, bool do_optimize)
{
  int error= 0;
  ulonglong local_testflag= param->testflag;
  bool optimize_done= !do_optimize, statistics_done= 0, full_repair_done= 0;
  const char *old_proc_info= thd->proc_info;
  char fixed_name[FN_REFLEN];
  MARIA_SHARE *share= file->s;
  ha_rows rows= file->state->records;
  TRN *old_trn= file->trn;
  my_bool locking= 0;
  DBUG_ENTER("ha_maria::repair");

  /*
    Normally this method is entered with a properly opened table. If the
    repair fails, it can be repeated with more elaborate options. Under
    special circumstances it can happen that a repair fails so that it
    closed the data file and cannot re-open it. In this case file->dfile
    is set to -1. We must not try another repair without an open data
    file. (Bug #25289)
  */
  if (file->dfile.file == -1)
  {
    sql_print_information("Retrying repair of: '%s' failed. "
                          "Please try REPAIR EXTENDED or aria_chk",
                          table->s->path.str);
    DBUG_RETURN(HA_ADMIN_FAILED);
  }

  /*
    If transactions was not enabled for a transactional table then
    file->s->status is not up to date. This is needed for repair_by_sort
    to work
  */
  if (share->base.born_transactional && !share->now_transactional)
    _ma_copy_nontrans_state_information(file);

  param->db_name= table->s->db.str;
  param->table_name= table->alias.c_ptr();
  param->tmpfile_createflag= O_RDWR | O_TRUNC;
  param->using_global_keycache= 1;
  param->thd= thd;
  param->tmpdir= &mysql_tmpdir_list;
  param->out_flag= 0;
  share->state.dupp_key= MI_MAX_KEY;
  strmov(fixed_name, share->open_file_name.str);
  unmap_file(file);

  /*
    Don't lock tables if we have used LOCK TABLE or if we come from
    enable_index()
  */
  if (!thd->locked_tables_mode && ! (param->testflag & T_NO_LOCKS))
  {
    locking= 1;
    if (maria_lock_database(file, table->s->tmp_table ? F_EXTRA_LCK : F_WRLCK))
    {
      _ma_check_print_error(param, ER_THD(thd, ER_CANT_LOCK), my_errno);
      DBUG_RETURN(HA_ADMIN_FAILED);
    }
  }

  if (!do_optimize ||
      (((share->data_file_type == BLOCK_RECORD) ?
        (share->state.changed & STATE_NOT_OPTIMIZED_ROWS) :
        (file->state->del ||
         share->state.split != file->state->records)) &&
       (!(param->testflag & T_QUICK) ||
        (share->state.changed & (STATE_NOT_OPTIMIZED_KEYS |
                                 STATE_NOT_OPTIMIZED_ROWS)))))
  {
    ulonglong key_map= ((local_testflag & T_CREATE_MISSING_KEYS) ?
                        maria_get_mask_all_keys_active(share->base.keys) :
                        share->state.key_map);
    ulonglong save_testflag= param->testflag;
    if (maria_test_if_sort_rep(file, file->state->records, key_map, 0) &&
        (local_testflag & T_REP_BY_SORT))
    {
      local_testflag |= T_STATISTICS;
      param->testflag |= T_STATISTICS;           // We get this for free
      statistics_done= 1;
      /* TODO: Remove BLOCK_RECORD test when parallel works with blocks */
      if (THDVAR(thd,repair_threads) > 1 &&
          share->data_file_type != BLOCK_RECORD)
      {
        char buf[40];
        /* TODO: respect maria_repair_threads variable */
        my_snprintf(buf, 40, "Repair with %d threads", my_count_bits(key_map));
        thd_proc_info(thd, buf);
        param->testflag|= T_REP_PARALLEL;
        error= maria_repair_parallel(param, file, fixed_name,
                                     MY_TEST(param->testflag & T_QUICK));
        /* to reset proc_info, as it was pointing to local buffer */
        thd_proc_info(thd, "Repair done");
      }
      else
      {
        thd_proc_info(thd, "Repair by sorting");
        param->testflag|= T_REP_BY_SORT;
        error= maria_repair_by_sort(param, file, fixed_name,
                                    MY_TEST(param->testflag & T_QUICK));
      }
      if (error && file->create_unique_index_by_sort &&
          share->state.dupp_key != MAX_KEY)
      {
        my_errno= HA_ERR_FOUND_DUPP_KEY;
        print_keydup_error(table, &table->key_info[share->state.dupp_key],
                           MYF(0));
      }
    }
    else
    {
      thd_proc_info(thd, "Repair with keycache");
      param->testflag &= ~(T_REP_BY_SORT | T_REP_PARALLEL);
      error= maria_repair(param, file, fixed_name,
                          MY_TEST(param->testflag & T_QUICK));
    }
    param->testflag= save_testflag | (param->testflag & T_RETRY_WITHOUT_QUICK);
    optimize_done= 1;
    /*
      set full_repair_done if we re-wrote all rows and all keys
      (and thus removed all transid's from the table
    */
    full_repair_done= !MY_TEST(param->testflag & T_QUICK);
  }
  if (!error)
  {
    if ((local_testflag & T_SORT_INDEX) &&
        (share->state.changed & STATE_NOT_SORTED_PAGES))
    {
      optimize_done= 1;
      thd_proc_info(thd, "Sorting index");
      error= maria_sort_index(param, file, fixed_name);
    }
    if (!error && !statistics_done && (local_testflag & T_STATISTICS))
    {
      if (share->state.changed & STATE_NOT_ANALYZED)
      {
        optimize_done= 1;
        thd_proc_info(thd, "Analyzing");
        error= maria_chk_key(param, file);
      }
      else
        local_testflag &= ~T_STATISTICS;        // Don't update statistics
    }
  }
  thd_proc_info(thd, "Saving state");
  if (full_repair_done && !error &&
      !(param->testflag & T_NO_CREATE_RENAME_LSN))
  {
    /* Set trid (needed if the table was moved from another system) */
    share->state.create_trid= trnman_get_min_safe_trid();
  }
  mysql_mutex_lock(&share->intern_lock);
  if (!error)
  {
    if ((share->state.changed & STATE_CHANGED) || maria_is_crashed(file))
    {
      DBUG_PRINT("info", ("Resetting crashed state"));
      share->state.changed&= ~(STATE_CHANGED | STATE_CRASHED_FLAGS |
                               STATE_IN_REPAIR | STATE_MOVED);
      file->update |= HA_STATE_CHANGED | HA_STATE_ROW_CHANGED;
    }
    /*
      repair updates share->state.state. Ensure that file->state is up to date
    */
    if (file->state != &share->state.state)
      *file->state= share->state.state;

    if (share->base.auto_key)
      _ma_update_auto_increment_key(param, file, 1);
    if (optimize_done)
      error= maria_update_state_info(param, file,
                                     UPDATE_TIME | UPDATE_OPEN_COUNT |
                                     (local_testflag &
                                      T_STATISTICS ? UPDATE_STAT : 0));
    /* File is repaired; Mark the file as moved to this system */
    (void) _ma_set_uuid(share, 0);

    info(HA_STATUS_NO_LOCK | HA_STATUS_TIME | HA_STATUS_VARIABLE |
         HA_STATUS_CONST);
    if (rows != file->state->records && !(param->testflag & T_VERY_SILENT))
    {
      char llbuff[22], llbuff2[22];
      _ma_check_print_warning(param, "Number of rows changed from %s to %s",
                              llstr(rows, llbuff),
                              llstr(file->state->records, llbuff2));
    }
  }
  else
  {
    maria_mark_crashed_on_repair(file);
    file->update |= HA_STATE_CHANGED | HA_STATE_ROW_CHANGED;
    maria_update_state_info(param, file, 0);
  }
  mysql_mutex_unlock(&share->intern_lock);
  thd_proc_info(thd, old_proc_info);
  thd_progress_end(thd);                        // Mark done
  if (locking)
    maria_lock_database(file, F_UNLCK);

  /* Reset trn, that may have been set by repair */
  if (old_trn && old_trn != file->trn)
    _ma_set_trn_for_table(file, old_trn);
  error= error ? HA_ADMIN_FAILED :
    (optimize_done ?
     (write_log_record_for_repair(param, file) ? HA_ADMIN_FAILED :
      HA_ADMIN_OK) : HA_ADMIN_ALREADY_DONE);
  DBUG_RETURN(error);
}