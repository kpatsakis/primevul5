static int maria_rollback(handlerton *hton __attribute__ ((unused)),
                          THD *thd, bool all)
{
  TRN *trn= THD_TRN;
  DBUG_ENTER("maria_rollback");

  DBUG_ASSERT(trnman_has_locked_tables(trn) == 0);
  trnman_reset_locked_tables(trn, 0);
  /* statement or transaction ? */
  if ((thd->variables.option_bits & (OPTION_NOT_AUTOCOMMIT | OPTION_BEGIN)) &&
      !all)
  {
    trnman_rollback_statement(trn);
    DBUG_RETURN(0); // end of statement
  }
  reset_thd_trn(thd, (MARIA_HA*) trn->used_instances);
  DBUG_RETURN(trnman_rollback_trn(trn) ?
              HA_ERR_OUT_OF_MEM : 0); // end of transaction
}