sub_select_postjoin_aggr(JOIN *join, JOIN_TAB *join_tab, bool end_of_records)
{
  enum_nested_loop_state rc;
  AGGR_OP *aggr= join_tab->aggr;

  /* This function cannot be called if join_tab has no associated aggregation */
  DBUG_ASSERT(aggr != NULL);

  DBUG_ENTER("sub_select_aggr_tab");

  if (join->thd->killed)
  {
    /* The user has aborted the execution of the query */
    join->thd->send_kill_message();
    DBUG_RETURN(NESTED_LOOP_KILLED);
  }

  if (end_of_records)
  {
    rc= aggr->end_send();
    if (rc >= NESTED_LOOP_OK)
      rc= sub_select(join, join_tab, end_of_records);
    DBUG_RETURN(rc);
  }

  rc= aggr->put_record();

  DBUG_RETURN(rc);
}