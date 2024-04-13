JOIN_TAB *next_top_level_tab(JOIN *join, JOIN_TAB *tab)
{
  tab= next_breadth_first_tab(join->first_breadth_first_tab(),
                              join->top_join_tab_count, tab);
  if (tab && tab->bush_root_tab)
    tab= NULL;
  return tab;
}