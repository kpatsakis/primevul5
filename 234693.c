void check_join_cache_usage_for_tables(JOIN *join, ulonglong options,
                                       uint no_jbuf_after)
{
  JOIN_TAB *tab;
  JOIN_TAB *prev_tab;

  for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
    tab->used_join_cache_level= join->max_allowed_join_cache_level;  
  }

  uint idx= join->const_tables;
  for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
restart:
    tab->icp_other_tables_ok= TRUE;
    tab->idx_cond_fact_out= TRUE;
    
    /* 
      Check if we have a preceding join_tab, as something that will feed us
      records that we could buffer. We don't have it, if 
       - this is the first non-const table in the join order,
       - this is the first table inside an SJM nest.
    */
    prev_tab= tab - 1;
    if (tab == join->join_tab + join->const_tables ||
        (tab->bush_root_tab && tab->bush_root_tab->bush_children->start == tab))
      prev_tab= NULL;

    switch (tab->type) {
    case JT_SYSTEM:
    case JT_CONST:
    case JT_EQ_REF:
    case JT_REF:
    case JT_REF_OR_NULL:
    case JT_ALL:
      tab->used_join_cache_level= check_join_cache_usage(tab, options,
                                                         no_jbuf_after,
                                                         idx,
                                                         prev_tab);
      tab->use_join_cache= MY_TEST(tab->used_join_cache_level);
      /*
        psergey-merge: todo: raise the question that this is really stupid that
        we can first allocate a join buffer, then decide not to use it and free
        it.
      */
      if (join->return_tab)
      {
        tab= join->return_tab;
        goto restart;
      }
      break; 
    default:
      tab->used_join_cache_level= 0;
    }
    if (!tab->bush_children)
      idx++;
  }
}