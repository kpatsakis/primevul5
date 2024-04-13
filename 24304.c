open_and_process_routine(THD *thd, Query_tables_list *prelocking_ctx,
                         Sroutine_hash_entry *rt,
                         Prelocking_strategy *prelocking_strategy,
                         bool has_prelocking_list,
                         Open_table_context *ot_ctx,
                         bool *need_prelocking, bool *routine_modifies_data)
{
  MDL_key::enum_mdl_namespace mdl_type= rt->mdl_request.key.mdl_namespace();
  DBUG_ENTER("open_and_process_routine");

  *routine_modifies_data= false;

  switch (mdl_type)
  {
  case MDL_key::FUNCTION:
  case MDL_key::PROCEDURE:
    {
      sp_head *sp;
      /*
        Try to get MDL lock on the routine.
        Note that we do not take locks on top-level CALLs as this can
        lead to a deadlock. Not locking top-level CALLs does not break
        the binlog as only the statements in the called procedure show
        up there, not the CALL itself.
      */
      if (rt != (Sroutine_hash_entry*)prelocking_ctx->sroutines_list.first ||
          mdl_type != MDL_key::PROCEDURE)
      {
        /*
          Since we acquire only shared lock on routines we don't
          need to care about global intention exclusive locks.
        */
        DBUG_ASSERT(rt->mdl_request.type == MDL_SHARED);

        /*
          Waiting for a conflicting metadata lock to go away may
          lead to a deadlock, detected by MDL subsystem.
          If possible, we try to resolve such deadlocks by releasing all
          metadata locks and restarting the pre-locking process.
          To prevent the error from polluting the diagnostics area
          in case of successful resolution, install a special error
          handler for ER_LOCK_DEADLOCK error.
        */
        MDL_deadlock_handler mdl_deadlock_handler(ot_ctx);

        thd->push_internal_handler(&mdl_deadlock_handler);
        bool result= thd->mdl_context.acquire_lock(&rt->mdl_request,
                                                   ot_ctx->get_timeout());
        thd->pop_internal_handler();

        if (result)
          DBUG_RETURN(TRUE);

        DEBUG_SYNC(thd, "after_shared_lock_pname");

        /* Ensures the routine is up-to-date and cached, if exists. */
        if (sp_cache_routine(thd, rt, has_prelocking_list, &sp))
          DBUG_RETURN(TRUE);

        /* Remember the version of the routine in the parse tree. */
        if (check_and_update_routine_version(thd, rt, sp))
          DBUG_RETURN(TRUE);

        /* 'sp' is NULL when there is no such routine. */
        if (sp)
        {
          *routine_modifies_data= sp->modifies_data();

          if (!has_prelocking_list)
            prelocking_strategy->handle_routine(thd, prelocking_ctx, rt, sp,
                                                need_prelocking);
        }
      }
      else
      {
        /*
          If it's a top level call, just make sure we have a recent
          version of the routine, if it exists.
          Validating routine version is unnecessary, since CALL
          does not affect the prepared statement prelocked list.
        */
        if (sp_cache_routine(thd, rt, FALSE, &sp))
          DBUG_RETURN(TRUE);
      }
    }
    break;
  case MDL_key::TRIGGER:
    /**
      We add trigger entries to lex->sroutines_list, but we don't
      load them here. The trigger entry is only used when building
      a transitive closure of objects used in a statement, to avoid
      adding to this closure objects that are used in the trigger more
      than once.
      E.g. if a trigger trg refers to table t2, and the trigger table t1
      is used multiple times in the statement (say, because it's used in
      function f1() twice), we will only add t2 once to the list of
      tables to prelock.

      We don't take metadata locks on triggers either: they are protected
      by a respective lock on the table, on which the trigger is defined.

      The only two cases which give "trouble" are SHOW CREATE TRIGGER
      and DROP TRIGGER statements. For these, statement syntax doesn't
      specify the table on which this trigger is defined, so we have
      to make a "dirty" read in the data dictionary to find out the
      table name. Once we discover the table name, we take a metadata
      lock on it, and this protects all trigger operations.
      Of course the table, in theory, may disappear between the dirty
      read and metadata lock acquisition, but in that case we just return
      a run-time error.

      Grammar of other trigger DDL statements (CREATE, DROP) requires
      the table to be specified explicitly, so we use the table metadata
      lock to protect trigger metadata in these statements. Similarly, in
      DML we always use triggers together with their tables, and thus don't
      need to take separate metadata locks on them.
    */
    break;
  default:
    /* Impossible type value. */
    DBUG_ASSERT(0);
  }
  DBUG_RETURN(FALSE);
}