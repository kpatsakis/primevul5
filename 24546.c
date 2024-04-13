int ha_maria::delete_all_rows()
{
  THD *thd= table->in_use;
  TRN *trn= file->trn;
  CHECK_UNTIL_WE_FULLY_IMPLEMENTED_VERSIONING("TRUNCATE in WRITE CONCURRENT");
#ifdef EXTRA_DEBUG
  if (trn && ! (trnman_get_flags(trn) & TRN_STATE_INFO_LOGGED))
  {
    trnman_set_flags(trn, trnman_get_flags(trn) | TRN_STATE_INFO_LOGGED |
                     TRN_STATE_TABLES_CAN_CHANGE);
    (void) translog_log_debug_info(trn, LOGREC_DEBUG_INFO_QUERY,
                                   (uchar*) thd->query(), thd->query_length());
  }
#endif
  /*
    If we are under LOCK TABLES, we have to do a commit as
    delete_all_rows() can't be rolled back
  */
  if (table->in_use->locked_tables_mode && trn &&
      trnman_has_locked_tables(trn))
  {
    int error;
    if ((error= implicit_commit(thd, 1)))
      return error;
  }

  /* Note that this can't be rolled back */
  return maria_delete_all_rows(file);
}