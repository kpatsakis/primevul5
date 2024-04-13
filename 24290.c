void Locked_tables_list::
unlink_all_closed_tables(THD *thd, MYSQL_LOCK *lock, size_t reopen_count)
{
  /* If we managed to take a lock, unlock tables and free the lock. */
  if (lock)
    mysql_unlock_tables(thd, lock);
  /*
    If a failure happened in reopen_tables(), we may have succeeded
    reopening some tables, but not all.
    This works when the connection was killed in mysql_lock_tables().
  */
  if (reopen_count)
  {
    while (reopen_count--)
    {
      /*
        When closing the table, we must remove it
        from thd->open_tables list.
        We rely on the fact that open_table() that was used
        in reopen_tables() always links the opened table
        to the beginning of the open_tables list.
      */
      DBUG_ASSERT(thd->open_tables == m_reopen_array[reopen_count]->table);

      thd->open_tables->pos_in_locked_tables->table= NULL;
      thd->open_tables->pos_in_locked_tables= 0;

      close_thread_table(thd, &thd->open_tables);
    }
  }
  /* Exclude all closed tables from the LOCK TABLES list. */
  for (TABLE_LIST *table_list= m_locked_tables; table_list; table_list=
       table_list->next_global)
  {
    if (table_list->table == NULL)
    {
      /* Unlink from list. */
      *table_list->prev_global= table_list->next_global;
      if (table_list->next_global == NULL)
        m_locked_tables_last= table_list->prev_global;
      else
        table_list->next_global->prev_global= table_list->prev_global;
      m_locked_tables_count--;
    }
  }

  /* If no tables left, do an automatic UNLOCK TABLES */
  if (thd->lock && thd->lock->table_count == 0)
    unlock_locked_tables(thd);
}