int ha_maria::analyze(THD *thd, HA_CHECK_OPT * check_opt)
{
  int error= 0;
  HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
  MARIA_SHARE *share= file->s;
  const char *old_proc_info;

  if (!param)
    return HA_ADMIN_INTERNAL_ERROR;

  maria_chk_init(param);
  param->thd= thd;
  param->op_name= "analyze";
  param->db_name= table->s->db.str;
  param->table_name= table->alias.c_ptr();
  param->testflag= (T_FAST | T_CHECK | T_SILENT | T_STATISTICS |
                   T_DONT_CHECK_CHECKSUM);
  param->using_global_keycache= 1;
  param->stats_method= (enum_handler_stats_method)THDVAR(thd,stats_method);

  if (!(share->state.changed & STATE_NOT_ANALYZED))
    return HA_ADMIN_ALREADY_DONE;

  old_proc_info= thd_proc_info(thd, "Scanning");
  thd_progress_init(thd, 1);
  error= maria_chk_key(param, file);
  if (!error)
  {
    mysql_mutex_lock(&share->intern_lock);
    error= maria_update_state_info(param, file, UPDATE_STAT);
    mysql_mutex_unlock(&share->intern_lock);
  }
  else if (!maria_is_crashed(file) && !thd->killed)
    maria_mark_crashed(file);
  thd_proc_info(thd, old_proc_info);
  thd_progress_end(thd);
  return error ? HA_ADMIN_CORRUPT : HA_ADMIN_OK;
}