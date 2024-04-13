bool close_cached_tables(THD *thd, TABLE_LIST *tables,
                         bool wait_for_refresh, ulong timeout)
{
  DBUG_ENTER("close_cached_tables");
  DBUG_ASSERT(thd || (!wait_for_refresh && !tables));
  DBUG_ASSERT(wait_for_refresh || !tables);

  if (!tables)
  {
    /* Free tables that are not used */
    purge_tables(false);
    if (!wait_for_refresh)
      DBUG_RETURN(false);
  }

  DBUG_PRINT("info", ("open table definitions: %d",
                      (int) tdc_records()));

  if (thd->locked_tables_mode)
  {
    /*
      If we are under LOCK TABLES, we need to reopen the tables without
      opening a door for any concurrent threads to sneak in and get
      lock on our tables. To achieve this we use exclusive metadata
      locks.
    */
    TABLE_LIST *tables_to_reopen= (tables ? tables :
                                  thd->locked_tables_list.locked_tables());
    bool result= false;

    /* close open HANDLER for this thread to allow table to be closed */
    mysql_ha_flush_tables(thd, tables_to_reopen);

    for (TABLE_LIST *table_list= tables_to_reopen; table_list;
         table_list= table_list->next_global)
    {
      int err;
      /* A check that the table was locked for write is done by the caller. */
      TABLE *table= find_table_for_mdl_upgrade(thd, table_list->db.str,
                                            table_list->table_name.str, &err);

      /* May return NULL if this table has already been closed via an alias. */
      if (! table)
        continue;

      if (thd->mdl_context.upgrade_shared_lock(table->mdl_ticket, MDL_EXCLUSIVE,
                                               timeout))
      {
        result= true;
        break;
      }
      table->file->extra(HA_EXTRA_PREPARE_FOR_FORCED_CLOSE);
      close_all_tables_for_name(thd, table->s, HA_EXTRA_NOT_USED, NULL);
    }
    /*
      No other thread has the locked tables open; reopen them and get the
      old locks. This should always succeed (unless some external process
      has removed the tables)
    */
    if (thd->locked_tables_list.reopen_tables(thd, false))
      result= true;

    /*
      Since downgrade_lock() won't do anything with shared
      metadata lock it is much simpler to go through all open tables rather
      than picking only those tables that were flushed.
    */
    for (TABLE *tab= thd->open_tables; tab; tab= tab->next)
      tab->mdl_ticket->downgrade_lock(MDL_SHARED_NO_READ_WRITE);

    DBUG_RETURN(result);
  }
  else if (tables)
  {
    /*
      Get an explicit MDL lock for all requested tables to ensure they are
      not used by any other thread
    */
    MDL_request_list mdl_requests;

    DBUG_PRINT("info", ("Waiting for other threads to close their open tables"));
    DEBUG_SYNC(thd, "after_flush_unlock");

    /* close open HANDLER for this thread to allow table to be closed */
    mysql_ha_flush_tables(thd, tables);

    for (TABLE_LIST *table= tables; table; table= table->next_local)
    {
      MDL_request *mdl_request= new (thd->mem_root) MDL_request;
      if (mdl_request == NULL)
        DBUG_RETURN(true);
      mdl_request->init(&table->mdl_request.key, MDL_EXCLUSIVE, MDL_STATEMENT);
      mdl_requests.push_front(mdl_request);
    }

    if (thd->mdl_context.acquire_locks(&mdl_requests, timeout))
      DBUG_RETURN(true);

    for (TABLE_LIST *table= tables; table; table= table->next_local)
      tdc_remove_table(thd, TDC_RT_REMOVE_ALL, table->db.str,
                       table->table_name.str, false);
  }
  DBUG_RETURN(false);
}