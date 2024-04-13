int join_init_read_record(JOIN_TAB *tab)
{
  /* 
    Note: the query plan tree for the below operations is constructed in
    save_agg_explain_data.
  */
  if (tab->distinct && tab->remove_duplicates())  // Remove duplicates.
    return 1;
  if (tab->filesort && tab->sort_table())     // Sort table.
    return 1;

  DBUG_EXECUTE_IF("kill_join_init_read_record",
                  tab->join->thd->set_killed(KILL_QUERY););
  if (tab->select && tab->select->quick && tab->select->quick->reset())
  {
    /* Ensures error status is propagated back to client */
    report_error(tab->table,
                 tab->join->thd->killed ? HA_ERR_QUERY_INTERRUPTED : HA_ERR_OUT_OF_MEM);
    return 1;
  }
  /* make sure we won't get ER_QUERY_INTERRUPTED from any code below */
  DBUG_EXECUTE_IF("kill_join_init_read_record",
                  tab->join->thd->reset_killed(););
  if (!tab->preread_init_done  && tab->preread_init())
    return 1;
  if (init_read_record(&tab->read_record, tab->join->thd, tab->table,
                       tab->select, tab->filesort_result, 1,1, FALSE))
    return 1;
  return (*tab->read_record.read_record)(&tab->read_record);
}