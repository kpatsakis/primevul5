int ha_maria::enable_indexes(uint mode)
{
  int error;
  ha_rows start_rows= file->state->records;
  DBUG_PRINT("info", ("ha_maria::enable_indexes mode: %d", mode));
  if (maria_is_all_keys_active(file->s->state.key_map, file->s->base.keys))
  {
    /* All indexes are enabled already. */
    return 0;
  }

  if (mode == HA_KEY_SWITCH_ALL)
  {
    error= maria_enable_indexes(file);
    /*
       Do not try to repair on error,
       as this could make the enabled state persistent,
       but mode==HA_KEY_SWITCH_ALL forbids it.
    */
  }
  else if (mode == HA_KEY_SWITCH_NONUNIQ_SAVE)
  {
    THD *thd= table->in_use;
    HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
    if (!param)
      return HA_ADMIN_INTERNAL_ERROR;

    const char *save_proc_info= thd_proc_info(thd, "Creating index");

    maria_chk_init(param);
    param->op_name= "recreating_index";
    param->testflag= (T_SILENT | T_REP_BY_SORT | T_QUICK |
                     T_CREATE_MISSING_KEYS | T_SAFE_REPAIR);
    /*
      Don't lock and unlock table if it's locked.
      Normally table should be locked.  This test is mostly for safety.
    */
    if (likely(file->lock_type != F_UNLCK))
      param->testflag|= T_NO_LOCKS;

    if (file->create_unique_index_by_sort)
      param->testflag|= T_CREATE_UNIQUE_BY_SORT;

    if (bulk_insert_single_undo == BULK_INSERT_SINGLE_UNDO_AND_NO_REPAIR)
    {
      bulk_insert_single_undo= BULK_INSERT_SINGLE_UNDO_AND_REPAIR;
      /*
        Don't bump create_rename_lsn, because UNDO_BULK_INSERT
        should not be skipped in case of crash during repair.
      */
      param->testflag|= T_NO_CREATE_RENAME_LSN;
    }

    param->myf_rw &= ~MY_WAIT_IF_FULL;
    param->orig_sort_buffer_length= THDVAR(thd,sort_buffer_size);
    param->stats_method= (enum_handler_stats_method)THDVAR(thd,stats_method);
    param->tmpdir= &mysql_tmpdir_list;
    if ((error= (repair(thd, param, 0) != HA_ADMIN_OK)) && param->retry_repair)
    {
      sql_print_warning("Warning: Enabling keys got errno %d on %s.%s, "
                        "retrying",
                        my_errno, param->db_name, param->table_name);
      /* This should never fail normally */
      DBUG_ASSERT(thd->killed != 0);
      /* Repairing by sort failed. Now try standard repair method. */
      param->testflag &= ~T_REP_BY_SORT;
      file->state->records= start_rows;
      error= (repair(thd, param, 0) != HA_ADMIN_OK);
      /*
        If the standard repair succeeded, clear all error messages which
        might have been set by the first repair. They can still be seen
        with SHOW WARNINGS then.
      */
      if (!error)
        thd->clear_error();
    }
    info(HA_STATUS_CONST);
    thd_proc_info(thd, save_proc_info);
  }
  else
  {
    /* mode not implemented */
    error= HA_ERR_WRONG_COMMAND;
  }
  DBUG_EXECUTE_IF("maria_flush_whole_log",
                  {
                    DBUG_PRINT("maria_flush_whole_log", ("now"));
                    translog_flush(translog_get_horizon());
                  });
  DBUG_EXECUTE_IF("maria_crash_enable_index",
                  {
                    DBUG_PRINT("maria_crash_enable_index", ("now"));
                    DBUG_SUICIDE();
                  });
  return error;
}