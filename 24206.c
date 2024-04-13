open_and_process_table(THD *thd, TABLE_LIST *tables, uint *counter, uint flags,
                       Prelocking_strategy *prelocking_strategy,
                       bool has_prelocking_list, Open_table_context *ot_ctx)
{
  bool error= FALSE;
  bool safe_to_ignore_table= FALSE;
  LEX *lex= thd->lex;
  DBUG_ENTER("open_and_process_table");
  DEBUG_SYNC(thd, "open_and_process_table");

  /*
    Ignore placeholders for derived tables. After derived tables
    processing, link to created temporary table will be put here.
    If this is derived table for view then we still want to process
    routines used by this view.
  */
  if (tables->derived)
  {
    if (!tables->view)
    {
      if (!tables->is_derived())
        tables->set_derived();
      goto end;
    }
    /*
      We restore view's name and database wiped out by derived tables
      processing and fall back to standard open process in order to
      obtain proper metadata locks and do other necessary steps like
      stored routine processing.
    */
    tables->db= tables->view_db.str;
    tables->db_length= tables->view_db.length;
    tables->table_name= tables->view_name.str;
    tables->table_name_length= tables->view_name.length;
  }

  if (!tables->derived &&
      is_infoschema_db(tables->db, tables->db_length))
  {
    /*
      Check whether the information schema contains a table
      whose name is tables->schema_table_name
    */
    ST_SCHEMA_TABLE *schema_table= tables->schema_table;
    if (!schema_table ||
        (schema_table->hidden &&
         ((sql_command_flags[lex->sql_command] & CF_STATUS_COMMAND) == 0 ||
          /*
            this check is used for show columns|keys from I_S hidden table
          */
          lex->sql_command == SQLCOM_SHOW_FIELDS ||
          lex->sql_command == SQLCOM_SHOW_KEYS)))
    {
      my_error(ER_UNKNOWN_TABLE, MYF(0),
               tables->table_name, INFORMATION_SCHEMA_NAME.str);
      DBUG_RETURN(1);
    }
  }
  /*
    If this TABLE_LIST object is a placeholder for an information_schema
    table, create a temporary table to represent the information_schema
    table in the query. Do not fill it yet - will be filled during
    execution.
  */
  if (tables->schema_table)
  {
    /*
      If this information_schema table is merged into a mergeable
      view, ignore it for now -- it will be filled when its respective
      TABLE_LIST is processed. This code works only during re-execution.
    */
    if (tables->view)
    {
      MDL_ticket *mdl_ticket;
      /*
        We still need to take a MDL lock on the merged view to protect
        it from concurrent changes.
      */
      if (!open_table_get_mdl_lock(thd, ot_ctx, &tables->mdl_request,
                                   flags, &mdl_ticket) &&
          mdl_ticket != NULL)
        goto process_view_routines;
      /* Fall-through to return error. */
    }
    else if (!mysql_schema_table(thd, lex, tables) &&
             !check_and_update_table_version(thd, tables, tables->table->s))
    {
      goto end;
    }
    error= TRUE;
    goto end;
  }
  DBUG_PRINT("tcache", ("opening table: '%s'.'%s'  item: %p",
                        tables->db, tables->table_name, tables)); //psergey: invalid read of size 1 here
  (*counter)++;

  /*
    Not a placeholder: must be a base/temporary table or a view. Let us open it.
  */
  if (tables->table)
  {
    /*
      If this TABLE_LIST object has an associated open TABLE object
      (TABLE_LIST::table is not NULL), that TABLE object must be a pre-opened
      temporary table.
    */
    DBUG_ASSERT(is_temporary_table(tables));
  }
  else if (tables->open_type == OT_TEMPORARY_ONLY)
  {
    /*
      OT_TEMPORARY_ONLY means that we are in CREATE TEMPORARY TABLE statement.
      Also such table list element can't correspond to prelocking placeholder
      or to underlying table of merge table.
      So existing temporary table should have been preopened by this moment
      and we can simply continue without trying to open temporary or base
      table.
    */
    DBUG_ASSERT(tables->open_strategy);
    DBUG_ASSERT(!tables->prelocking_placeholder);
    DBUG_ASSERT(!tables->parent_l);
    DBUG_RETURN(0);
  }

  /* Not a placeholder: must be a base table or a view. Let us open it. */
  if (tables->prelocking_placeholder)
  {
    /*
      For the tables added by the pre-locking code, attempt to open
      the table but fail silently if the table does not exist.
      The real failure will occur when/if a statement attempts to use
      that table.
    */
    No_such_table_error_handler no_such_table_handler;
    thd->push_internal_handler(&no_such_table_handler);

    /*
      We're opening a table from the prelocking list.

      Since this table list element might have been added after pre-opening
      of temporary tables we have to try to open temporary table for it.

      We can't simply skip this table list element and postpone opening of
      temporary table till the execution of substatement for several reasons:
      - Temporary table can be a MERGE table with base underlying tables,
        so its underlying tables has to be properly open and locked at
        prelocking stage.
      - Temporary table can be a MERGE table and we might be in PREPARE
        phase for a prepared statement. In this case it is important to call
        HA_ATTACH_CHILDREN for all merge children.
        This is necessary because merge children remember "TABLE_SHARE ref type"
        and "TABLE_SHARE def version" in the HA_ATTACH_CHILDREN operation.
        If HA_ATTACH_CHILDREN is not called, these attributes are not set.
        Then, during the first EXECUTE, those attributes need to be updated.
        That would cause statement re-preparing (because changing those
        attributes during EXECUTE is caught by THD::m_reprepare_observers).
        The problem is that since those attributes are not set in merge
        children, another round of PREPARE will not help.
    */
    error= thd->open_temporary_table(tables);

    if (!error && !tables->table)
      error= open_table(thd, tables, ot_ctx);

    thd->pop_internal_handler();
    safe_to_ignore_table= no_such_table_handler.safely_trapped_errors();
  }
  else if (tables->parent_l && (thd->open_options & HA_OPEN_FOR_REPAIR))
  {
    /*
      Also fail silently for underlying tables of a MERGE table if this
      table is opened for CHECK/REPAIR TABLE statement. This is needed
      to provide complete list of problematic underlying tables in
      CHECK/REPAIR TABLE output.
    */
    Repair_mrg_table_error_handler repair_mrg_table_handler;
    thd->push_internal_handler(&repair_mrg_table_handler);

    error= thd->open_temporary_table(tables);

    if (!error && !tables->table)
      error= open_table(thd, tables, ot_ctx);

    thd->pop_internal_handler();
    safe_to_ignore_table= repair_mrg_table_handler.safely_trapped_errors();
  }
  else
  {
    if (tables->parent_l)
    {
      /*
        Even if we are opening table not from the prelocking list we
        still might need to look for a temporary table if this table
        list element corresponds to underlying table of a merge table.
      */
      error= thd->open_temporary_table(tables);
    }

    if (!error && !tables->table)
      error= open_table(thd, tables, ot_ctx);
  }

  if (error)
  {
    if (! ot_ctx->can_recover_from_failed_open() && safe_to_ignore_table)
    {
      DBUG_PRINT("info", ("open_table: ignoring table '%s'.'%s'",
                          tables->db, tables->alias));
      error= FALSE;
    }
    goto end;
  }

  /*
    We can't rely on simple check for TABLE_LIST::view to determine
    that this is a view since during re-execution we might reopen
    ordinary table in place of view and thus have TABLE_LIST::view
    set from repvious execution and TABLE_LIST::table set from
    current.
  */
  if (!tables->table && tables->view)
  {
    /* VIEW placeholder */
    (*counter)--;

    /*
      tables->next_global list consists of two parts:
      1) Query tables and underlying tables of views.
      2) Tables used by all stored routines that this statement invokes on
         execution.
      We need to know where the bound between these two parts is. If we've
      just opened a view, which was the last table in part #1, and it
      has added its base tables after itself, adjust the boundary pointer
      accordingly.
    */
    if (lex->query_tables_own_last == &(tables->next_global) &&
        tables->view->query_tables)
      lex->query_tables_own_last= tables->view->query_tables_last;
    /*
      Let us free memory used by 'sroutines' hash here since we never
      call destructor for this LEX.
    */
    my_hash_free(&tables->view->sroutines);
    goto process_view_routines;
  }

  /*
    Special types of open can succeed but still don't set
    TABLE_LIST::table to anything.
  */
  if (tables->open_strategy && !tables->table)
    goto end;

  error= extend_table_list(thd, tables, prelocking_strategy, has_prelocking_list);
  if (error)
    goto end;

  /* Copy grant information from TABLE_LIST instance to TABLE one. */
  tables->table->grant= tables->grant;

  /* Check and update metadata version of a base table. */
  error= check_and_update_table_version(thd, tables, tables->table->s);

  if (error)
    goto end;
  /*
    After opening a MERGE table add the children to the query list of
    tables, so that they are opened too.
    Note that placeholders don't have the handler open.
  */
  /* MERGE tables need to access parent and child TABLE_LISTs. */
  DBUG_ASSERT(tables->table->pos_in_table_list == tables);
  /* Non-MERGE tables ignore this call. */
  if (tables->table->file->extra(HA_EXTRA_ADD_CHILDREN_LIST))
  {
    error= TRUE;
    goto end;
  }

process_view_routines:
  /*
    Again we may need cache all routines used by this view and add
    tables used by them to table list.
  */
  if (tables->view &&
      thd->locked_tables_mode <= LTM_LOCK_TABLES &&
      ! has_prelocking_list)
  {
    bool need_prelocking= FALSE;
    TABLE_LIST **save_query_tables_last= lex->query_tables_last;

    error= prelocking_strategy->handle_view(thd, lex, tables,
                                            &need_prelocking);

    if (need_prelocking && ! lex->requires_prelocking())
      lex->mark_as_requiring_prelocking(save_query_tables_last);

    if (error)
      goto end;
  }

end:
  DBUG_RETURN(error);
}