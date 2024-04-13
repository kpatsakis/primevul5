static int maria_commit(handlerton *hton __attribute__ ((unused)),
                        THD *thd, bool all)
{
  TRN *trn= THD_TRN;
  int res= 0;
  MARIA_HA *used_instances;
  DBUG_ENTER("maria_commit");

  /* No commit inside lock_tables() */
  if ((!trn ||
       thd->locked_tables_mode == LTM_LOCK_TABLES ||
       thd->locked_tables_mode == LTM_PRELOCKED_UNDER_LOCK_TABLES))
    DBUG_RETURN(0);

  /* statement or transaction ? */
  if ((thd->variables.option_bits & (OPTION_NOT_AUTOCOMMIT | OPTION_BEGIN)) &&
      !all)
    DBUG_RETURN(0); // end of statement

  used_instances= (MARIA_HA*) trn->used_instances;
  trnman_reset_locked_tables(trn, 0);
  trnman_set_flags(trn, trnman_get_flags(trn) & ~TRN_STATE_INFO_LOGGED);
  trn->used_instances= 0;
  if (ma_commit(trn))
    res= HA_ERR_COMMIT_ERROR;
  reset_thd_trn(thd, used_instances);
  thd_set_ha_data(thd, maria_hton, 0);
  DBUG_RETURN(res);
}