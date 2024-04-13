bool open_and_lock_tables(THD *thd, const DDL_options_st &options,
                          TABLE_LIST *tables,
                          bool derived, uint flags,
                          Prelocking_strategy *prelocking_strategy)
{
  uint counter;
  MDL_savepoint mdl_savepoint= thd->mdl_context.mdl_savepoint();
  DBUG_ENTER("open_and_lock_tables");
  DBUG_PRINT("enter", ("derived handling: %d", derived));

  if (open_tables(thd, options, &tables, &counter, flags, prelocking_strategy))
    goto err;

  DBUG_EXECUTE_IF("sleep_open_and_lock_after_open", {
                  const char *old_proc_info= thd->proc_info;
                  thd->proc_info= "DBUG sleep";
                  my_sleep(6000000);
                  thd->proc_info= old_proc_info;});

  if (lock_tables(thd, tables, counter, flags))
    goto err;

  (void) read_statistics_for_tables_if_needed(thd, tables);
  
  if (derived)
  {
    if (mysql_handle_derived(thd->lex, DT_INIT))
      goto err;
    if (thd->prepare_derived_at_open &&
        (mysql_handle_derived(thd->lex, DT_PREPARE)))
      goto err;
  }

  DBUG_RETURN(FALSE);
err:
  if (! thd->in_sub_stmt)
    trans_rollback_stmt(thd);  /* Necessary if derived handling failed. */
  close_thread_tables(thd);
  /* Don't keep locks for a failed statement. */
  thd->mdl_context.rollback_to_savepoint(mdl_savepoint);
  DBUG_RETURN(TRUE);
}