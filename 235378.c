lock_table_names(THD *thd, const DDL_options_st &options,
                 TABLE_LIST *tables_start, TABLE_LIST *tables_end,
                 ulong lock_wait_timeout, uint flags)
{
  MDL_request_list mdl_requests;
  TABLE_LIST *table;
  MDL_request global_request;
  MDL_savepoint mdl_savepoint;
  DBUG_ENTER("lock_table_names");

  DBUG_ASSERT(!thd->locked_tables_mode);

  for (table= tables_start; table && table != tables_end;
       table= table->next_global)
  {
    DBUG_PRINT("info", ("mdl_request.type: %d  open_type: %d",
                        table->mdl_request.type, table->open_type));
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
      schema_request->init(MDL_key::SCHEMA, table->db.str, "",
                           MDL_INTENTION_EXCLUSIVE,
                           MDL_TRANSACTION);
      mdl_requests.push_front(schema_request);
    }

    mdl_requests.push_front(&table->mdl_request);
  }

  if (mdl_requests.is_empty())
    DBUG_RETURN(FALSE);

  if (flags & MYSQL_OPEN_SKIP_SCOPED_MDL_LOCK)
  {
    DBUG_RETURN(thd->mdl_context.acquire_locks(&mdl_requests,
                                               lock_wait_timeout) ||
                upgrade_lock_if_not_exists(thd, options, tables_start,
                                           lock_wait_timeout));
  }

  /* Protect this statement against concurrent BACKUP STAGE or FTWRL. */
  if (thd->has_read_only_protection())
    DBUG_RETURN(true);

  global_request.init(MDL_key::BACKUP, "", "", MDL_BACKUP_DDL, MDL_STATEMENT);
  mdl_savepoint= thd->mdl_context.mdl_savepoint();

  while (!thd->mdl_context.acquire_locks(&mdl_requests, lock_wait_timeout) &&
         !upgrade_lock_if_not_exists(thd, options, tables_start,
                                     lock_wait_timeout) &&
         !thd->mdl_context.try_acquire_lock(&global_request))
  {
    if (global_request.ticket)
    {
      thd->mdl_backup_ticket= global_request.ticket;
      DBUG_RETURN(false);
    }

    /*
      There is ongoing or pending BACKUP STAGE or FTWRL.
      Wait until it finishes and re-try.
    */
    thd->mdl_context.rollback_to_savepoint(mdl_savepoint);
    if (thd->mdl_context.acquire_lock(&global_request, lock_wait_timeout))
      break;
    thd->mdl_context.rollback_to_savepoint(mdl_savepoint);

    /* Reset tickets for all acquired locks */
    global_request.ticket= 0;
    MDL_request_list::Iterator it(mdl_requests);
    while (auto mdl_request= it++)
      mdl_request->ticket= 0;
  }
  DBUG_RETURN(true);
}