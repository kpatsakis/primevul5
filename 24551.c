int ha_maria::optimize(THD * thd, HA_CHECK_OPT *check_opt)
{
  int error;
  HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);

  if (!file || !param)
    return HA_ADMIN_INTERNAL_ERROR;

  maria_chk_init(param);
  param->thd= thd;
  param->op_name= "optimize";
  param->testflag= (check_opt->flags | T_SILENT | T_FORCE_CREATE |
                   T_REP_BY_SORT | T_STATISTICS | T_SORT_INDEX);
  param->orig_sort_buffer_length= THDVAR(thd, sort_buffer_size);
  thd_progress_init(thd, 1);
  if ((error= repair(thd, param, 1)) && param->retry_repair)
  {
    sql_print_warning("Warning: Optimize table got errno %d on %s.%s, retrying",
                      my_errno, param->db_name, param->table_name);
    param->testflag &= ~T_REP_BY_SORT;
    error= repair(thd, param, 0);
  }
  thd_progress_end(thd);
  return error;
}