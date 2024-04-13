Locked_tables_list::reopen_tables(THD *thd, bool need_reopen)
{
  Open_table_context ot_ctx(thd, MYSQL_OPEN_REOPEN);
  size_t reopen_count= 0;
  MYSQL_LOCK *lock;
  MYSQL_LOCK *merged_lock;
  DBUG_ENTER("Locked_tables_list::reopen_tables");

  DBUG_ASSERT(some_table_marked_for_reopen || !need_reopen);


  /* Reset flag that some table was marked for reopen */
  some_table_marked_for_reopen= 0;

  for (TABLE_LIST *table_list= m_locked_tables;
       table_list; table_list= table_list->next_global)
  {
    if (need_reopen)
    {
      if (!table_list->table || !table_list->table->needs_reopen())
        continue;
      for (TABLE **prev= &thd->open_tables; *prev; prev= &(*prev)->next)
      {
        if (*prev == table_list->table)
        {
          thd->locked_tables_list.unlink_from_list(thd, table_list, false);
          mysql_lock_remove(thd, thd->lock, *prev);
          (*prev)->file->extra(HA_EXTRA_PREPARE_FOR_FORCED_CLOSE);
          close_thread_table(thd, prev);
          break;
        }
      }
      DBUG_ASSERT(table_list->table == NULL);
    }
    else
    {
      if (table_list->table)                      /* The table was not closed */
        continue;
    }

    DBUG_ASSERT(reopen_count < m_locked_tables_count);
    m_reopen_array[reopen_count++]= table_list;
  }
  if (reopen_count)
  {
    TABLE **tables= (TABLE**) my_alloca(reopen_count * sizeof(TABLE*));

    for (uint i= 0 ; i < reopen_count ; i++)
    {
      TABLE_LIST *table_list= m_reopen_array[i];
      /* Links into thd->open_tables upon success */
      if (open_table(thd, table_list, &ot_ctx))
      {
        unlink_all_closed_tables(thd, 0, i);
        my_afree((void*) tables);
        DBUG_RETURN(TRUE);
      }
      tables[i]= table_list->table;
      table_list->table->pos_in_locked_tables= table_list;
      /* See also the comment on lock type in init_locked_tables(). */
      table_list->table->reginfo.lock_type= table_list->lock_type;
    }

    thd->in_lock_tables= 1;
    /*
      We re-lock all tables with mysql_lock_tables() at once rather
      than locking one table at a time because of the case
      reported in Bug#45035: when the same table is present
      in the list many times, thr_lock.c fails to grant READ lock
      on a table that is already locked by WRITE lock, even if
      WRITE lock is taken by the same thread. If READ and WRITE
      lock are passed to thr_lock.c in the same list, everything
      works fine. Patching legacy code of thr_lock.c is risking to
      break something else.
    */
    lock= mysql_lock_tables(thd, tables, reopen_count,
                            MYSQL_OPEN_REOPEN);
    thd->in_lock_tables= 0;
    if (lock == NULL || (merged_lock=
                         mysql_lock_merge(thd->lock, lock)) == NULL)
    {
      unlink_all_closed_tables(thd, lock, reopen_count);
      if (! thd->killed)
        my_error(ER_LOCK_DEADLOCK, MYF(0));
      my_afree((void*) tables);
      DBUG_RETURN(TRUE);
    }
    thd->lock= merged_lock;
    my_afree((void*) tables);
  }
  DBUG_RETURN(FALSE);
}