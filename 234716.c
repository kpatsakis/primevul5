static JOIN_TAB *next_breadth_first_tab(JOIN_TAB *first_top_tab,
                                        uint n_top_tabs_count, JOIN_TAB *tab)
{
  n_top_tabs_count += tab->join->aggr_tables;
  if (!tab->bush_root_tab)
  {
    /* We're at top level. Get the next top-level tab */
    tab++;
    if (tab < first_top_tab + n_top_tabs_count)
      return tab;

    /* No more top-level tabs. Switch to enumerating SJM nest children */
    tab= first_top_tab;
  }
  else
  {
    /* We're inside of an SJM nest */
    if (!tab->last_leaf_in_bush)
    {
      /* There's one more table in the nest, return it. */
      return ++tab;
    }
    else
    {
      /* 
        There are no more tables in this nest. Get out of it and then we'll
        proceed to the next nest.
      */
      tab= tab->bush_root_tab + 1;
    }
  }
   
  /* 
    Ok, "tab" points to a top-level table, and we need to find the next SJM
    nest and enter it.
  */
  for (; tab < first_top_tab + n_top_tabs_count; tab++)
  {
    if (tab->bush_children)
      return tab->bush_children->start;
  }
  return NULL;
}