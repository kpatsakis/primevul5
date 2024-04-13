JOIN_TAB *first_depth_first_tab(JOIN* join)
{
  JOIN_TAB* tab;
  /* This means we're starting the enumeration */
  if (join->const_tables == join->top_join_tab_count || !join->join_tab)
    return NULL;

  tab= join->join_tab + join->const_tables;

  return (tab->bush_children) ? tab->bush_children->start : tab;
}