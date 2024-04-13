static int maria_rollback(handlerton *hton, THD *thd, bool all)
{
  TRN *trn= THD_TRN;
  DBUG_ENTER("maria_rollback");
  if (!trn)
    DBUG_RETURN(0);
  if (trn->undo_lsn)
    push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                        ER_DATA_WAS_COMMITED_UNDER_ROLLBACK,
                        ER_THD(thd, ER_DATA_WAS_COMMITED_UNDER_ROLLBACK),
                        "Aria");
  if (all)
    DBUG_RETURN(maria_commit(hton, thd, all));
  /* Statement rollbacks are ignored. Commit will happen in external_lock */
  DBUG_RETURN(0);
}