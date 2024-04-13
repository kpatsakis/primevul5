int ha_maria::rename_table(const char *from, const char *to)
{
  THD *thd= current_thd;
  (void) translog_log_debug_info(0, LOGREC_DEBUG_INFO_QUERY,
                                 (uchar*) thd->query(), thd->query_length());
  return maria_rename(from, to);
}