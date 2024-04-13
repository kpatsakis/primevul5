JOIN_TAB *next_depth_first_tab(JOIN* join, JOIN_TAB* tab)
{
  /* If we're inside SJM nest and have reached its end, get out */
  if (tab->last_leaf_in_bush)
    return tab->bush_root_tab;
  
  /* Move to next tab in the array we're traversing */
  tab++;
  
  if (tab == join->join_tab +join->top_join_tab_count)
    return NULL; /* Outside SJM nest and reached EOF */

  if (tab->bush_children)
    return tab->bush_children->start;

  return tab;
}