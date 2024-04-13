bool open_normal_and_derived_tables(THD *thd, TABLE_LIST *tables, uint flags,
                                    uint dt_phases)
{
  DML_prelocking_strategy prelocking_strategy;
  uint counter;
  MDL_savepoint mdl_savepoint= thd->mdl_context.mdl_savepoint();
  DBUG_ENTER("open_normal_and_derived_tables");
  if (open_tables(thd, &tables, &counter, flags, &prelocking_strategy) ||
      mysql_handle_derived(thd->lex, dt_phases))
    goto end;

  DBUG_RETURN(0);
end:
  /*
    No need to commit/rollback the statement transaction: it's
    either not started or we're filling in an INFORMATION_SCHEMA
    table on the fly, and thus mustn't manipulate with the
    transaction of the enclosing statement.
  */
  DBUG_ASSERT(thd->transaction.stmt.is_empty() ||
              (thd->state_flags & Open_tables_state::BACKUPS_AVAIL));
  close_thread_tables(thd);
  /* Don't keep locks for a failed statement. */
  thd->mdl_context.rollback_to_savepoint(mdl_savepoint);

  DBUG_RETURN(TRUE); /* purecov: inspected */
}