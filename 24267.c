Open_table_context::
request_backoff_action(enum_open_table_action action_arg,
                       TABLE_LIST *table)
{
  /*
    A back off action may be one of three kinds:

    * We met a broken table that needs repair, or a table that
      is not present on this MySQL server and needs re-discovery.
      To perform the action, we need an exclusive metadata lock on
      the table. Acquiring X lock while holding other shared
      locks can easily lead to deadlocks. We rely on MDL deadlock
      detector to discover them. If this is a multi-statement
      transaction that holds metadata locks for completed statements,
      we should keep these locks after discovery/repair.
      The action type in this case is OT_DISCOVER or OT_REPAIR.
    * Our attempt to acquire an MDL lock lead to a deadlock,
      detected by the MDL deadlock detector. The current
      session was chosen a victim. If this is a multi-statement
      transaction that holds metadata locks taken by completed
      statements, restarting locking for the current statement
      may lead to a livelock. Releasing locks of completed
      statements can not be done as will lead to violation
      of ACID. Thus, again, if m_has_locks is set,
      we report an error. Otherwise, when there are no metadata
      locks other than which belong to this statement, we can
      try to recover from error by releasing all locks and
      restarting the pre-locking.
      Similarly, a deadlock error can occur when the
      pre-locking process met a TABLE_SHARE that is being
      flushed, and unsuccessfully waited for the flush to
      complete. A deadlock in this case can happen, e.g.,
      when our session is holding a metadata lock that
      is being waited on by a session which is using
      the table which is being flushed. The only way
      to recover from this error is, again, to close all
      open tables, release all locks, and retry pre-locking.
      Action type name is OT_REOPEN_TABLES. Re-trying
      while holding some locks may lead to a livelock,
      and thus we don't do it.
    * Finally, this session has open TABLEs from different
      "generations" of the table cache. This can happen, e.g.,
      when, after this session has successfully opened one
      table used for a statement, FLUSH TABLES interfered and
      expelled another table used in it. FLUSH TABLES then
      blocks and waits on the table already opened by this
      statement.
      We detect this situation by ensuring that table cache
      version of all tables used in a statement is the same.
      If it isn't, all tables needs to be reopened.
      Note, that we can always perform a reopen in this case,
      even if we already have metadata locks, since we don't
      keep tables open between statements and a livelock
      is not possible.
  */
  if (action_arg == OT_BACKOFF_AND_RETRY && m_has_locks)
  {
    my_error(ER_LOCK_DEADLOCK, MYF(0));
    m_thd->mark_transaction_to_rollback(true);
    return TRUE;
  }
  /*
    If auto-repair or discovery are requested, a pointer to table
    list element must be provided.
  */
  if (table)
  {
    DBUG_ASSERT(action_arg == OT_DISCOVER || action_arg == OT_REPAIR);
    m_failed_table= (TABLE_LIST*) m_thd->alloc(sizeof(TABLE_LIST));
    if (m_failed_table == NULL)
      return TRUE;
    m_failed_table->init_one_table(table->db, table->db_length,
                                   table->table_name,
                                   table->table_name_length,
                                   table->alias, TL_WRITE);
    m_failed_table->open_strategy= table->open_strategy;
    m_failed_table->mdl_request.set_type(MDL_EXCLUSIVE);
  }
  m_action= action_arg;
  return FALSE;
}