int ha_maria::check(THD * thd, HA_CHECK_OPT * check_opt)
{
  int error, fatal_error;
  HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
  MARIA_SHARE *share= file->s;
  const char *old_proc_info;
  TRN *old_trn= file->trn;

  if (!file || !param) return HA_ADMIN_INTERNAL_ERROR;

  unmap_file(file);
  register_handler(file);
  maria_chk_init(param);
  param->thd= thd;
  param->op_name= "check";
  param->db_name= table->s->db.str;
  param->table_name= table->alias.c_ptr();
  param->testflag= check_opt->flags | T_CHECK | T_SILENT;
  param->stats_method= (enum_handler_stats_method)THDVAR(thd,stats_method);

  if (!(table->db_stat & HA_READ_ONLY))
    param->testflag |= T_STATISTICS;
  param->using_global_keycache= 1;

  if (!maria_is_crashed(file) &&
      (((param->testflag & T_CHECK_ONLY_CHANGED) &&
        !(share->state.changed & (STATE_CHANGED | STATE_CRASHED_FLAGS |
                                  STATE_IN_REPAIR)) &&
        share->state.open_count == 0) ||
       ((param->testflag & T_FAST) && (share->state.open_count ==
                                      (uint) (share->global_changed ? 1 :
                                              0)))))
    return HA_ADMIN_ALREADY_DONE;

  maria_chk_init_for_check(param, file);
  param->max_allowed_lsn= translog_get_horizon();

  if ((file->s->state.changed & (STATE_CRASHED_FLAGS | STATE_MOVED)) ==
      STATE_MOVED)
  {
    _ma_check_print_error(param, "%s", zerofill_error_msg);
    return HA_ADMIN_CORRUPT;
  }

  old_proc_info= thd_proc_info(thd, "Checking status");
  thd_progress_init(thd, 3);
  error= maria_chk_status(param, file);                // Not fatal
  /* maria_chk_size() will flush the page cache for this file */
  if (maria_chk_size(param, file))
    error= 1;
  if (!error)
    error|= maria_chk_del(param, file, param->testflag);
  thd_proc_info(thd, "Checking keys");
  thd_progress_next_stage(thd);
  if (!error)
    error= maria_chk_key(param, file);
  thd_proc_info(thd, "Checking data");
  thd_progress_next_stage(thd);
  if (!error)
  {
    if ((!(param->testflag & T_QUICK) &&
         ((share->options &
           (HA_OPTION_PACK_RECORD | HA_OPTION_COMPRESS_RECORD)) ||
          (param->testflag & (T_EXTEND | T_MEDIUM)))) || maria_is_crashed(file))
    {
      ulonglong old_testflag= param->testflag;
      param->testflag |= T_MEDIUM;

      /* BLOCK_RECORD does not need a cache as it is using the page cache */
      if (file->s->data_file_type != BLOCK_RECORD)
        error= init_io_cache(&param->read_cache, file->dfile.file,
                             my_default_record_cache_size, READ_CACHE,
                             share->pack.header_length, 1, MYF(MY_WME));
      if (!error)
        error= maria_chk_data_link(param, file,
                                   MY_TEST(param->testflag & T_EXTEND));

      if (file->s->data_file_type != BLOCK_RECORD)
        end_io_cache(&param->read_cache);
      param->testflag= old_testflag;
    }
  }
  fatal_error= error;
  if (param->error_printed &&
      param->error_printed == (param->skip_lsn_error_count +
                               param->not_visible_rows_found) &&
      !(share->state.changed & (STATE_CRASHED_FLAGS | STATE_IN_REPAIR)))
  {
    _ma_check_print_error(param, "%s", zerofill_error_msg);
    /* This ensures that a future REPAIR TABLE will only do a zerofill */
    file->update|= STATE_MOVED;
    share->state.changed|= STATE_MOVED;
    fatal_error= 0;
  }
  if (!fatal_error)
  {
    if ((share->state.changed & (STATE_CHANGED | STATE_MOVED |
                                 STATE_CRASHED_FLAGS |
                                 STATE_IN_REPAIR | STATE_NOT_ANALYZED)) ||
        (param->testflag & T_STATISTICS) || maria_is_crashed(file))
    {
      file->update |= HA_STATE_CHANGED | HA_STATE_ROW_CHANGED;
      mysql_mutex_lock(&share->intern_lock);
      DBUG_PRINT("info", ("Resetting crashed state"));
      share->state.changed&= ~(STATE_CHANGED | STATE_CRASHED_FLAGS |
                               STATE_IN_REPAIR);
      if (!(table->db_stat & HA_READ_ONLY))
      {
        int tmp;
        if ((tmp= maria_update_state_info(param, file,
                                          UPDATE_TIME | UPDATE_OPEN_COUNT |
                                          UPDATE_STAT)))
          error= tmp;
      }
      mysql_mutex_unlock(&share->intern_lock);
      info(HA_STATUS_NO_LOCK | HA_STATUS_TIME | HA_STATUS_VARIABLE |
           HA_STATUS_CONST);

      /*
        Write a 'table is ok' message to error log if table is ok and
        we have written to error log that table was getting checked
      */
      if (!error && !(table->db_stat & HA_READ_ONLY) &&
          !maria_is_crashed(file) && thd->error_printed_to_log &&
          (param->warning_printed || param->error_printed ||
           param->note_printed))
        _ma_check_print_info(param, "Table is fixed");
    }
  }
  else if (!maria_is_crashed(file) && !thd->killed)
  {
    maria_mark_crashed(file);
    file->update |= HA_STATE_CHANGED | HA_STATE_ROW_CHANGED;
  }

  /* Reset trn, that may have been set by repair */
  if (old_trn && old_trn != file->trn)
  {
    DBUG_ASSERT(old_trn->used_instances == 0);
    _ma_set_trn_for_table(file, old_trn);
  }
  thd_proc_info(thd, old_proc_info);
  thd_progress_end(thd);
  return error ? HA_ADMIN_CORRUPT : HA_ADMIN_OK;
}