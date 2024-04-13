bool open_tables(THD *thd, const DDL_options_st &options,
                 TABLE_LIST **start, uint *counter, uint flags,
                 Prelocking_strategy *prelocking_strategy)
{
  /*
    We use pointers to "next_global" member in the last processed
    TABLE_LIST element and to the "next" member in the last processed
    Sroutine_hash_entry element as iterators over, correspondingly,
    the table list and stored routines list which stay valid and allow
    to continue iteration when new elements are added to the tail of
    the lists.
  */
  TABLE_LIST **table_to_open;
  Sroutine_hash_entry **sroutine_to_open;
  TABLE_LIST *tables;
  Open_table_context ot_ctx(thd, flags);
  bool error= FALSE;
  bool some_routine_modifies_data= FALSE;
  bool has_prelocking_list;
  DBUG_ENTER("open_tables");

  /* Data access in XA transaction is only allowed when it is active. */
  for (TABLE_LIST *table= *start; table; table= table->next_global)
    if (!table->schema_table)
    {
      if (thd->transaction.xid_state.check_has_uncommitted_xa())
      {
	thd->transaction.xid_state.er_xaer_rmfail();
        DBUG_RETURN(true);
      }
      else
        break;
    }

  thd->current_tablenr= 0;
restart:
  /*
    Close HANDLER tables which are marked for flush or against which there
    are pending exclusive metadata locks. This is needed both in order to
    avoid deadlocks and to have a point during statement execution at
    which such HANDLERs are closed even if they don't create problems for
    the current session (i.e. to avoid having a DDL blocked by HANDLERs
    opened for a long time).
  */
  if (thd->handler_tables_hash.records)
    mysql_ha_flush(thd);

  has_prelocking_list= thd->lex->requires_prelocking();
  table_to_open= start;
  sroutine_to_open= &thd->lex->sroutines_list.first;
  *counter= 0;
  THD_STAGE_INFO(thd, stage_opening_tables);
  prelocking_strategy->reset(thd);

  /*
    If we are executing LOCK TABLES statement or a DDL statement
    (in non-LOCK TABLES mode) we might have to acquire upgradable
    semi-exclusive metadata locks (SNW or SNRW) on some of the
    tables to be opened.
    When executing CREATE TABLE .. If NOT EXISTS .. SELECT, the
    table may not yet exist, in which case we acquire an exclusive
    lock.
    We acquire all such locks at once here as doing this in one
    by one fashion may lead to deadlocks or starvation. Later when
    we will be opening corresponding table pre-acquired metadata
    lock will be reused (thanks to the fact that in recursive case
    metadata locks are acquired without waiting).
  */
  if (! (flags & (MYSQL_OPEN_HAS_MDL_LOCK |
                  MYSQL_OPEN_FORCE_SHARED_MDL |
                  MYSQL_OPEN_FORCE_SHARED_HIGH_PRIO_MDL)))
  {
    if (thd->locked_tables_mode)
    {
      /*
        Under LOCK TABLES, we can't acquire new locks, so we instead
        need to check if appropriate locks were pre-acquired.
      */
      if (open_tables_check_upgradable_mdl(thd, *start,
                                           thd->lex->first_not_own_table(),
                                           flags))
      {
        error= TRUE;
        goto error;
      }
    }
    else
    {
      TABLE_LIST *table;
      if (lock_table_names(thd, options, *start,
                           thd->lex->first_not_own_table(),
                           ot_ctx.get_timeout(), flags))
      {
        error= TRUE;
        goto error;
      }
      for (table= *start; table && table != thd->lex->first_not_own_table();
           table= table->next_global)
      {
        if (table->mdl_request.type >= MDL_SHARED_UPGRADABLE)
          table->mdl_request.ticket= NULL;
      }
    }
  }

  /*
    Perform steps of prelocking algorithm until there are unprocessed
    elements in prelocking list/set.
  */
  while (*table_to_open  ||
         (thd->locked_tables_mode <= LTM_LOCK_TABLES && *sroutine_to_open))
  {
    /*
      For every table in the list of tables to open, try to find or open
      a table.
    */
    for (tables= *table_to_open; tables;
         table_to_open= &tables->next_global, tables= tables->next_global)
    {
      error= open_and_process_table(thd, tables, counter, flags,
                                    prelocking_strategy, has_prelocking_list,
                                    &ot_ctx);

      if (unlikely(error))
      {
        if (ot_ctx.can_recover_from_failed_open())
        {
          /*
            We have met exclusive metadata lock or old version of table.
            Now we have to close all tables and release metadata locks.
            We also have to throw away set of prelocked tables (and thus
            close tables from this set that were open by now) since it
            is possible that one of tables which determined its content
            was changed.

            Instead of implementing complex/non-robust logic mentioned
            above we simply close and then reopen all tables.

            We have to save pointer to table list element for table which we
            have failed to open since closing tables can trigger removal of
            elements from the table list (if MERGE tables are involved),
          */
          close_tables_for_reopen(thd, start, ot_ctx.start_of_statement_svp());

          /*
            Here we rely on the fact that 'tables' still points to the valid
            TABLE_LIST element. Altough currently this assumption is valid
            it may change in future.
          */
          if (ot_ctx.recover_from_failed_open())
            goto error;

          /* Re-open temporary tables after close_tables_for_reopen(). */
          if (thd->open_temporary_tables(*start))
            goto error;

          error= FALSE;
          goto restart;
        }
        goto error;
      }

      DEBUG_SYNC(thd, "open_tables_after_open_and_process_table");
    }

    /*
      If we are not already in prelocked mode and extended table list is
      not yet built for our statement we need to cache routines it uses
      and build the prelocking list for it.
      If we are not in prelocked mode but have built the extended table
      list, we still need to call open_and_process_routine() to take
      MDL locks on the routines.
    */
    if (thd->locked_tables_mode <= LTM_LOCK_TABLES)
    {
      /*
        Process elements of the prelocking set which are present there
        since parsing stage or were added to it by invocations of
        Prelocking_strategy methods in the above loop over tables.

        For example, if element is a routine, cache it and then,
        if prelocking strategy prescribes so, add tables it uses to the
        table list and routines it might invoke to the prelocking set.
      */
      for (Sroutine_hash_entry *rt= *sroutine_to_open; rt;
           sroutine_to_open= &rt->next, rt= rt->next)
      {
        bool need_prelocking= false;
        bool routine_modifies_data;
        TABLE_LIST **save_query_tables_last= thd->lex->query_tables_last;

        error= open_and_process_routine(thd, thd->lex, rt, prelocking_strategy,
                                        has_prelocking_list, &ot_ctx,
                                        &need_prelocking,
                                        &routine_modifies_data);

        // Remember if any of SF modifies data.
        some_routine_modifies_data|= routine_modifies_data;

        if (need_prelocking && ! thd->lex->requires_prelocking())
          thd->lex->mark_as_requiring_prelocking(save_query_tables_last);

        if (need_prelocking && ! *start)
          *start= thd->lex->query_tables;

        if (unlikely(error))
        {
          if (ot_ctx.can_recover_from_failed_open())
          {
            close_tables_for_reopen(thd, start,
                                    ot_ctx.start_of_statement_svp());
            if (ot_ctx.recover_from_failed_open())
              goto error;

            /* Re-open temporary tables after close_tables_for_reopen(). */
            if (thd->open_temporary_tables(*start))
              goto error;

            error= FALSE;
            goto restart;
          }
          /*
            Serious error during reading stored routines from mysql.proc table.
            Something is wrong with the table or its contents, and an error has
            been emitted; we must abort.
          */
          goto error;
        }
      }
    }
    if ((error= prelocking_strategy->handle_end(thd)))
      goto error;
  }

  /*
    After successful open of all tables, including MERGE parents and
    children, attach the children to their parents. At end of statement,
    the children are detached. Attaching and detaching are always done,
    even under LOCK TABLES.

    We also convert all TL_WRITE_DEFAULT and TL_READ_DEFAULT locks to
    appropriate "real" lock types to be used for locking and to be passed
    to storage engine.

    And start wsrep TOI if needed.
  */
  for (tables= *start; tables; tables= tables->next_global)
  {
    TABLE *tbl= tables->table;

    if (!tbl)
      continue;

    /* Schema tables may not have a TABLE object here. */
    if (tbl->file->ha_table_flags() & HA_CAN_MULTISTEP_MERGE)
    {
      /* MERGE tables need to access parent and child TABLE_LISTs. */
      DBUG_ASSERT(tbl->pos_in_table_list == tables);
      if (tbl->file->extra(HA_EXTRA_ATTACH_CHILDREN))
      {
        error= TRUE;
        goto error;
      }
    }

    /* Set appropriate TABLE::lock_type. */
    if (tbl && tables->lock_type != TL_UNLOCK && !thd->locked_tables_mode)
    {
      if (tables->lock_type == TL_WRITE_DEFAULT)
        tbl->reginfo.lock_type= thd->update_lock_default;
      else if (tables->lock_type == TL_READ_DEFAULT)
          tbl->reginfo.lock_type=
            read_lock_type_for_table(thd, thd->lex, tables,
                                     some_routine_modifies_data);
      else
        tbl->reginfo.lock_type= tables->lock_type;
    }
  }

#ifdef WITH_WSREP
  if (WSREP(thd)                                       &&
      wsrep_replicate_myisam                           &&
      (*start)                                         &&
      (*start)->table                                  &&
      (*start)->table->file->ht == myisam_hton         &&
      wsrep_thd_is_local(thd)                          &&
      !is_stat_table(&(*start)->db, &(*start)->alias)  &&
      thd->get_command() != COM_STMT_PREPARE           &&
      !thd->stmt_arena->is_stmt_prepare()              &&
      ((thd->lex->sql_command == SQLCOM_INSERT         ||
        thd->lex->sql_command == SQLCOM_INSERT_SELECT  ||
        thd->lex->sql_command == SQLCOM_REPLACE        ||
        thd->lex->sql_command == SQLCOM_REPLACE_SELECT ||
        thd->lex->sql_command == SQLCOM_UPDATE         ||
        thd->lex->sql_command == SQLCOM_UPDATE_MULTI   ||
        thd->lex->sql_command == SQLCOM_LOAD           ||
        thd->lex->sql_command == SQLCOM_DELETE)))
  {
      wsrep_before_rollback(thd, true);
      wsrep_after_rollback(thd, true);
      wsrep_after_statement(thd);
      WSREP_TO_ISOLATION_BEGIN(NULL, NULL, (*start));
  }
#endif /* WITH_WSREP */

error:
#ifdef WITH_WSREP
wsrep_error_label:
#endif
  THD_STAGE_INFO(thd, stage_after_opening_tables);
  thd_proc_info(thd, 0);

  if (unlikely(error) && *table_to_open)
  {
    (*table_to_open)->table= NULL;
  }
  DBUG_PRINT("open_tables", ("returning: %d", (int) error));
  DBUG_RETURN(error);
}