void multi_update::abort_result_set()
{
  /* the error was handled or nothing deleted and no side effects return */
  if (unlikely(error_handled ||
               (!thd->transaction.stmt.modified_non_trans_table && !updated)))
    return;

  /* Something already updated so we have to invalidate cache */
  if (updated)
    query_cache_invalidate3(thd, update_tables, 1);
  /*
    If all tables that has been updated are trans safe then just do rollback.
    If not attempt to do remaining updates.
  */

  if (! trans_safe)
  {
    DBUG_ASSERT(thd->transaction.stmt.modified_non_trans_table);
    if (do_update && table_count > 1)
    {
      /* Add warning here */
      (void) do_updates();
    }
  }
  if (thd->transaction.stmt.modified_non_trans_table)
  {
    /*
      The query has to binlog because there's a modified non-transactional table
      either from the query's list or via a stored routine: bug#13270,23333
    */
    if (WSREP_EMULATE_BINLOG(thd) || mysql_bin_log.is_open())
    {
      /*
        THD::killed status might not have been set ON at time of an error
        got caught and if happens later the killed error is written
        into repl event.
      */
      int errcode= query_error_code(thd, thd->killed == NOT_KILLED);
      /* the error of binary logging is ignored */
      (void)thd->binlog_query(THD::ROW_QUERY_TYPE,
                        thd->query(), thd->query_length(),
                        transactional_tables, FALSE, FALSE, errcode);
    }
    thd->transaction.all.modified_non_trans_table= TRUE;
  }
  thd->transaction.all.m_unsafe_rollback_flags|=
    (thd->transaction.stmt.m_unsafe_rollback_flags & THD_TRANS::DID_WAIT);
  DBUG_ASSERT(trans_safe || !updated || thd->transaction.stmt.modified_non_trans_table);
}