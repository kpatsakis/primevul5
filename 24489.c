static int maria_create_trn_for_mysql(MARIA_HA *info)
{
  THD *thd= ((TABLE*) info->external_ref)->in_use;
  TRN *trn= THD_TRN;
  DBUG_ENTER("maria_create_trn_for_mysql");

  if (!trn)  /* no transaction yet - open it now */
  {
    trn= trnman_new_trn(& thd->transaction->wt);
    if (unlikely(!trn))
      DBUG_RETURN(HA_ERR_OUT_OF_MEM);
    thd_set_ha_data(thd, maria_hton, trn);
    if (thd->variables.option_bits & (OPTION_NOT_AUTOCOMMIT | OPTION_BEGIN))
      trans_register_ha(thd, TRUE, maria_hton, trn->trid);
  }
  _ma_set_trn_for_table(info, trn);
  if (!trnman_increment_locked_tables(trn))
  {
    trans_register_ha(thd, FALSE, maria_hton, trn->trid);
    trnman_new_statement(trn);
  }
#ifdef EXTRA_DEBUG
  if (info->lock_type == F_WRLCK &&
      ! (trnman_get_flags(trn) & TRN_STATE_INFO_LOGGED))
  {
    trnman_set_flags(trn, trnman_get_flags(trn) | TRN_STATE_INFO_LOGGED |
                     TRN_STATE_TABLES_CAN_CHANGE);
    (void) translog_log_debug_info(trn, LOGREC_DEBUG_INFO_QUERY,
                                   (uchar*) thd->query(),
                                   thd->query_length());
  }
  else
  {
    DBUG_PRINT("info", ("lock_type: %d  trnman_flags: %u",
                        info->lock_type, trnman_get_flags(trn)));
  }

#endif
  DBUG_RETURN(0);
}