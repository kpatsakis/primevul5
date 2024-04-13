void close_tables_for_reopen(THD *thd, TABLE_LIST **tables,
                             const MDL_savepoint &start_of_statement_svp)
{
  TABLE_LIST *first_not_own_table= thd->lex->first_not_own_table();
  TABLE_LIST *tmp;

  /*
    If table list consists only from tables from prelocking set, table list
    for new attempt should be empty, so we have to update list's root pointer.
  */
  if (first_not_own_table == *tables)
    *tables= 0;
  thd->lex->chop_off_not_own_tables();
  /* Reset MDL tickets for procedures/functions */
  for (Sroutine_hash_entry *rt=
         (Sroutine_hash_entry*)thd->lex->sroutines_list.first;
       rt; rt= rt->next)
    rt->mdl_request.ticket= NULL;
  sp_remove_not_own_routines(thd->lex);
  for (tmp= *tables; tmp; tmp= tmp->next_global)
  {
    tmp->table= 0;
    tmp->mdl_request.ticket= NULL;
    /* We have to cleanup translation tables of views. */
    tmp->cleanup_items();
  }
  /*
    No need to commit/rollback the statement transaction: it's
    either not started or we're filling in an INFORMATION_SCHEMA
    table on the fly, and thus mustn't manipulate with the
    transaction of the enclosing statement.
  */
  DBUG_ASSERT(thd->transaction.stmt.is_empty() ||
              (thd->state_flags & Open_tables_state::BACKUPS_AVAIL));
  close_thread_tables(thd);
  thd->mdl_context.rollback_to_savepoint(start_of_statement_svp);
}