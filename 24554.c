int ha_maria::delete_table(const char *name)
{
  THD *thd= current_thd;
  (void) translog_log_debug_info(0, LOGREC_DEBUG_INFO_QUERY,
                                 (uchar*) thd->query(), thd->query_length());
  return maria_delete_table(name);
}