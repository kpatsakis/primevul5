void JOIN::get_partial_cost_and_fanout(int end_tab_idx,
                                       table_map filter_map,
                                       double *read_time_arg, 
                                       double *record_count_arg)
{
  double record_count= 1;
  double read_time= 0.0;
  double sj_inner_fanout= 1.0;
  JOIN_TAB *end_tab= NULL;
  JOIN_TAB *tab;
  int i;
  int last_sj_table= MAX_TABLES;

  /* 
    Handle a special case where the join is degenerate, and produces no
    records
  */
  if (table_count == const_tables)
  {
    *read_time_arg= 0.0;
    /*
      We return 1, because 
       - it is the pessimistic estimate (there might be grouping)
       - it's safer, as we're less likely to hit the edge cases in
         calculations.
    */
    *record_count_arg=1.0;
    return;
  }

  for (tab= first_depth_first_tab(this), i= const_tables;
       tab;
       tab= next_depth_first_tab(this, tab), i++)
  {
    end_tab= tab;
    if (i == end_tab_idx)
      break;
  }

  for (tab= first_depth_first_tab(this), i= const_tables;
       ;
       tab= next_depth_first_tab(this, tab), i++)
  {
    if (end_tab->bush_root_tab && end_tab->bush_root_tab == tab)
    {
      /* 
        We've entered the SJM nest that contains the end_tab. The caller is
        - interested in fanout inside the nest (because that's how many times 
          we'll invoke the attached WHERE conditions)
        - not interested in cost
      */
      record_count= 1.0;
      read_time= 0.0;
    }
    
    /* 
      Ignore fanout (but not cost) from sj-inner tables, as long as 
      the range that processes them finishes before the end_tab
    */
    if (tab->sj_strategy != SJ_OPT_NONE)
    {
      sj_inner_fanout= 1.0;
      last_sj_table= i + tab->n_sj_tables;
    }
    
    table_map cur_table_map;
    if (tab->table)
      cur_table_map= tab->table->map;
    else
    {
      /* This is a SJ-Materialization nest. Check all of its tables */
      TABLE *first_child= tab->bush_children->start->table;
      TABLE_LIST *sjm_nest= first_child->pos_in_table_list->embedding;
      cur_table_map= sjm_nest->nested_join->used_tables;
    }
    if (tab->records_read && (cur_table_map & filter_map))
    {
      record_count= COST_MULT(record_count, tab->records_read);
      read_time= COST_ADD(read_time,
                          COST_ADD(tab->read_time,
                                   record_count / (double) TIME_FOR_COMPARE));
      if (tab->emb_sj_nest)
        sj_inner_fanout= COST_MULT(sj_inner_fanout, tab->records_read);
				     }

    if (i == last_sj_table)
    {
      record_count /= sj_inner_fanout;
      sj_inner_fanout= 1.0;
      last_sj_table= MAX_TABLES;
    }

    if (tab == end_tab)
      break;
  }
  *read_time_arg= read_time;// + record_count / TIME_FOR_COMPARE;
  *record_count_arg= record_count;
}