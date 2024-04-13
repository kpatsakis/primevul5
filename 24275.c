lock_table_names(THD *thd, const DDL_options_st &options,
                 TABLE_LIST *tables_start, TABLE_LIST *tables_end,
                 ulong lock_wait_timeout, uint flags)
{
  MDL_request_list mdl_requests;
  TABLE_LIST *table;
  MDL_request global_request;
  ulong org_lock_wait_timeout= lock_wait_timeout;
  /* Check if we are using CREATE TABLE ... IF NOT EXISTS */
  bool create_table;
  Dummy_error_handler error_handler;
  DBUG_ENTER("lock_table_names");

  DBUG_ASSERT(!thd->locked_tables_mode);

  for (table= tables_start; table && table != tables_end;
       table= table->next_global)
  {
    if (table->mdl_request.type < MDL_SHARED_UPGRADABLE ||
        table->mdl_request.type == MDL_SHARED_READ_ONLY ||
        table->open_type == OT_TEMPORARY_ONLY ||
        (table->open_type == OT_TEMPORARY_OR_BASE && is_temporary_table(table)))
    {
      continue;
    }

    /* Write lock on normal tables is not allowed in a read only transaction. */
    if (thd->tx_read_only)
    {
      my_error(ER_CANT_EXECUTE_IN_READ_ONLY_TRANSACTION, MYF(0));
      DBUG_RETURN(true);
    }

    /* Scoped locks: Take intention exclusive locks on all involved schemas. */
    if (!(flags & MYSQL_OPEN_SKIP_SCOPED_MDL_LOCK))
    {
      MDL_request *schema_request= new (thd->mem_root) MDL_request;
      if (schema_request == NULL)
        DBUG_RETURN(TRUE);
      schema_request->init(MDL_key::SCHEMA, table->db, "",
                           MDL_INTENTION_EXCLUSIVE,
                           MDL_TRANSACTION);
      mdl_requests.push_front(schema_request);
    }

    mdl_requests.push_front(&table->mdl_request);
  }

  if (mdl_requests.is_empty())
    DBUG_RETURN(FALSE);

  /* Check if CREATE TABLE without REPLACE was used */
  create_table= thd->lex->sql_command == SQLCOM_CREATE_TABLE &&
                !options.or_replace();

  if (!(flags & MYSQL_OPEN_SKIP_SCOPED_MDL_LOCK))
  {
    /*
      Protect this statement against concurrent global read lock
      by acquiring global intention exclusive lock with statement
      duration.
    */
    if (thd->global_read_lock.can_acquire_protection())
      DBUG_RETURN(TRUE);
    global_request.init(MDL_key::GLOBAL, "", "", MDL_INTENTION_EXCLUSIVE,
                        MDL_STATEMENT);
    mdl_requests.push_front(&global_request);

    if (create_table)
#ifdef WITH_WSREP
      if (!(thd->lex->sql_command == SQLCOM_CREATE_TABLE &&
	    thd->wsrep_exec_mode == REPL_RECV))
#endif
      lock_wait_timeout= 0;                     // Don't wait for timeout
  }

  for (;;)
  {
    if (create_table)
      thd->push_internal_handler(&error_handler);  // Avoid warnings & errors
    bool res= thd->mdl_context.acquire_locks(&mdl_requests, lock_wait_timeout);
    if (create_table)
      thd->pop_internal_handler();

    if (!res)
      DBUG_RETURN(FALSE);                       // Got locks

    if (!create_table)
      DBUG_RETURN(TRUE);                        // Return original error

    /*
      We come here in the case of lock timeout when executing CREATE TABLE.
      Verify that table does exist (it usually does, as we got a lock conflict)
    */
    if (ha_table_exists(thd, tables_start->db, tables_start->table_name))
    {
      if (options.if_not_exists())
      {
        push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                            ER_TABLE_EXISTS_ERROR,
                            ER_THD(thd, ER_TABLE_EXISTS_ERROR),
                            tables_start->table_name);
      }
      else
        my_error(ER_TABLE_EXISTS_ERROR, MYF(0), tables_start->table_name);
      DBUG_RETURN(TRUE);
    }
    /*
      We got error from acquire_locks, but the table didn't exists.
      This could happen if another connection runs a statement
      involving this non-existent table, and this statement took the mdl,
      but didn't error out with ER_NO_SUCH_TABLE yet (yes, a race condition).
      We play safe and restart the original acquire_locks with the
      original timeout.
    */
    create_table= 0;
    lock_wait_timeout= org_lock_wait_timeout;
  }
}