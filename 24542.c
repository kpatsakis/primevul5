int ha_maria::repair(THD * thd, HA_CHECK_OPT *check_opt)
{
  int error;
  HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
  ha_rows start_records;
  const char *old_proc_info;

  if (!file || !param)
    return HA_ADMIN_INTERNAL_ERROR;

  maria_chk_init(param);
  param->thd= thd;
  param->op_name= "repair";

  /*
    The following can only be true if the table was marked as STATE_MOVED
    during a CHECK TABLE and the table has not been used since then
  */
  if ((file->s->state.changed & STATE_MOVED) &&
      !(file->s->state.changed & STATE_CRASHED_FLAGS))
  {
    param->db_name= table->s->db.str;
    param->table_name= table->alias.c_ptr();
    _ma_check_print_info(param, "Running zerofill on moved table");
    return zerofill(thd, check_opt);
  }

  param->testflag= ((check_opt->flags & ~(T_EXTEND)) |
                   T_SILENT | T_FORCE_CREATE | T_CALC_CHECKSUM |
                   (check_opt->flags & T_EXTEND ? T_REP : T_REP_BY_SORT));
  param->orig_sort_buffer_length= THDVAR(thd, sort_buffer_size);
  param->backup_time= check_opt->start_time;
  start_records= file->state->records;
  old_proc_info= thd_proc_info(thd, "Checking table");
  thd_progress_init(thd, 1);
  while ((error= repair(thd, param, 0)) && param->retry_repair)
  {
    param->retry_repair= 0;
    file->state->records= start_records;
    if (test_all_bits(param->testflag,
                      (uint) (T_RETRY_WITHOUT_QUICK | T_QUICK)))
    {
      param->testflag&= ~(T_RETRY_WITHOUT_QUICK | T_QUICK);
      /* Ensure we don't loose any rows when retrying without quick */
      param->testflag|= T_SAFE_REPAIR;
      if (thd->vio_ok())
        _ma_check_print_info(param, "Retrying repair without quick");
      else
        sql_print_information("Retrying repair of: '%s' without quick",
                              table->s->path.str);
      continue;
    }
    param->testflag &= ~T_QUICK;
    if (param->testflag & T_REP_BY_SORT)
    {
      param->testflag= (param->testflag & ~T_REP_BY_SORT) | T_REP;
      if (thd->vio_ok())
        _ma_check_print_info(param, "Retrying repair with keycache");
      sql_print_information("Retrying repair of: '%s' with keycache",
                            table->s->path.str);
      continue;
    }
    break;
  }
  /*
    Commit is needed in the case of tables are locked to ensure that repair
    is registered in the recovery log
  */
  if (implicit_commit(thd, TRUE))
    error= HA_ADMIN_COMMIT_ERROR;

  if (!error && start_records != file->state->records &&
      !(check_opt->flags & T_VERY_SILENT))
  {
    char llbuff[22], llbuff2[22];
    sql_print_information("Found %s of %s rows when repairing '%s'",
                          llstr(file->state->records, llbuff),
                          llstr(start_records, llbuff2),
                          table->s->path.str);
  }
  thd_proc_info(thd, old_proc_info);
  thd_progress_end(thd);
  return error;
}