static uint make_join_orderinfo(JOIN *join)
{
  /*
    This function needs to be fixed to take into account that we now have SJM
    nests.
  */
  DBUG_ASSERT(0);

  JOIN_TAB *tab;
  if (join->need_tmp)
    return join->table_count;
  tab= join->get_sort_by_join_tab();
  return tab ? (uint)(tab-join->join_tab) : join->table_count;
}