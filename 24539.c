int ha_maria::implicit_commit(THD *thd, bool new_trn)
{
#ifndef MARIA_CANNOT_ROLLBACK
#error this method should be removed
#endif
  TRN *trn;
  int error;
  uint locked_tables;
  extern my_bool plugins_are_initialized;
  MARIA_HA *used_tables, *trn_next;
  DBUG_ENTER("ha_maria::implicit_commit");

  if (!maria_hton || !plugins_are_initialized || !(trn= THD_TRN))
    DBUG_RETURN(0);
  if (!new_trn && (thd->locked_tables_mode == LTM_LOCK_TABLES ||
                   thd->locked_tables_mode == LTM_PRELOCKED_UNDER_LOCK_TABLES))
  {
    /*
      No commit inside LOCK TABLES.

      Note that we come here only at the end of the top statement
      (dispatch_command()), we are never committing inside a sub-statement./
    */
    DBUG_PRINT("info", ("locked_tables, skipping"));
    DBUG_RETURN(0);
  }

  /* Prepare to move used_instances and locked tables to new TRN object */
  locked_tables= trnman_has_locked_tables(trn);
  trnman_reset_locked_tables(trn, 0);
  relink_trn_used_instances(&used_tables, trn);

  error= 0;
  if (unlikely(ma_commit(trn)))
    error= HA_ERR_COMMIT_ERROR;
  if (!new_trn)
  {
    reset_thd_trn(thd, used_tables);
    goto end;
  }

  /*
    We need to create a new transaction and put it in THD_TRN. Indeed,
    tables may be under LOCK TABLES, and so they will start the next
    statement assuming they have a trn (see ha_maria::start_stmt()).
  */
  trn= trnman_new_trn(& thd->transaction->wt);
  thd_set_ha_data(thd, maria_hton, trn);
  if (unlikely(trn == NULL))
  {
    reset_thd_trn(thd, used_tables);
    error= HA_ERR_OUT_OF_MEM;
    goto end;
  }
  /*
    Move all locked tables to the new transaction
    We must do it here as otherwise file->thd and file->state may be
    stale pointers. We can't do this in start_stmt() as we don't know
    when we should call _ma_setup_live_state() and in some cases, like
    in check table, we use the table without calling start_stmt().
  */

  for (MARIA_HA *handler= used_tables; handler ;
       handler= trn_next)
  {
    trn_next= handler->trn_next;
    DBUG_ASSERT(handler->s->base.born_transactional);

    /* If handler uses versioning */
    if (handler->s->lock_key_trees)
    {
      /* _ma_set_trn_for_table() will be called indirectly */
      if (_ma_setup_live_state(handler))
        error= HA_ERR_OUT_OF_MEM;
    }
    else
      _ma_set_trn_for_table(handler, trn);
  }
  /* This is just a commit, tables stay locked if they were: */
  trnman_reset_locked_tables(trn, locked_tables);

end:
  DBUG_RETURN(error);
}