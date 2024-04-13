void revise_cache_usage(JOIN_TAB *join_tab)
{
  JOIN_TAB *tab;
  JOIN_TAB *first_inner;

  if (join_tab->first_inner)
  {
    JOIN_TAB *end_tab= join_tab;
    for (first_inner= join_tab->first_inner; 
         first_inner;
         first_inner= first_inner->first_upper)           
    {
      for (tab= end_tab; tab >= first_inner; tab--)
        set_join_cache_denial(tab);
      end_tab= first_inner;
    }
  }
  else if (join_tab->first_sj_inner_tab)
  {
    first_inner= join_tab->first_sj_inner_tab;
    for (tab= join_tab; tab >= first_inner; tab--)
    {
      set_join_cache_denial(tab);
    }
  }
  else set_join_cache_denial(join_tab);
}