JOIN_TAB *first_explain_order_tab(JOIN* join)
{
  JOIN_TAB* tab;
  tab= join->join_tab;
  if (!tab)
    return NULL; /* Can happen when when the tables were optimized away */
  return (tab->bush_children) ? tab->bush_children->start : tab;
}