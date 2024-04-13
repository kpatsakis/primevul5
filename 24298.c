Locked_tables_list::unlock_locked_tables(THD *thd)
{
  DBUG_ASSERT(!thd->in_sub_stmt &&
              !(thd->state_flags & Open_tables_state::BACKUPS_AVAIL));
  /*
    Sic: we must be careful to not close open tables if
    we're not in LOCK TABLES mode: unlock_locked_tables() is
    sometimes called implicitly, expecting no effect on
    open tables, e.g. from begin_trans().
  */
  if (thd->locked_tables_mode != LTM_LOCK_TABLES)
    return;

  for (TABLE_LIST *table_list= m_locked_tables;
       table_list; table_list= table_list->next_global)
  {
    /*
      Clear the position in the list, the TABLE object will be
      returned to the table cache.
    */
    if (table_list->table)                    // If not closed
      table_list->table->pos_in_locked_tables= NULL;
  }
  thd->leave_locked_tables_mode();

  TRANSACT_TRACKER(clear_trx_state(thd, TX_LOCKED_TABLES));

  DBUG_ASSERT(thd->transaction.stmt.is_empty());
  close_thread_tables(thd);

  /*
    We rely on the caller to implicitly commit the
    transaction and release transactional locks.
  */

  /*
    After closing tables we can free memory used for storing lock
    request for metadata locks and TABLE_LIST elements.
  */
  reset();
}