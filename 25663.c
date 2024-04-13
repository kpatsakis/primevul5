bool JOIN::get_best_combination()
{
  uint tablenr;
  table_map used_tables;
  JOIN_TAB *j;
  KEYUSE *keyuse;
  DBUG_ENTER("get_best_combination");

   /*
    Additional plan nodes for postjoin tmp tables:
      1? + // For GROUP BY
      1? + // For DISTINCT
      1? + // For aggregation functions aggregated in outer query
           // when used with distinct
      1? + // For ORDER BY
      1?   // buffer result
    Up to 2 tmp tables are actually used, but it's hard to tell exact number
    at this stage.
  */ 
  uint aggr_tables= (group_list ? 1 : 0) +
                    (select_distinct ?
                     (tmp_table_param. using_outer_summary_function ? 2 : 1) : 0) +
                    (order ? 1 : 0) +
       (select_options & (SELECT_BIG_RESULT | OPTION_BUFFER_RESULT) ? 1 : 0) ;
  
  if (aggr_tables == 0)
    aggr_tables= 1; /* For group by pushdown */

  if (select_lex->window_specs.elements)
    aggr_tables++;

  if (aggr_tables > 2)
    aggr_tables= 2;
  if (!(join_tab= (JOIN_TAB*) thd->alloc(sizeof(JOIN_TAB)*
                                        (top_join_tab_count + aggr_tables))))
    DBUG_RETURN(TRUE);

  full_join=0;
  hash_join= FALSE;

  fix_semijoin_strategies_for_picked_join_order(this);
   
  JOIN_TAB_RANGE *root_range;
  if (!(root_range= new (thd->mem_root) JOIN_TAB_RANGE))
    DBUG_RETURN(TRUE);
   root_range->start= join_tab;
  /* root_range->end will be set later */
  join_tab_ranges.empty();

  if (join_tab_ranges.push_back(root_range, thd->mem_root))
    DBUG_RETURN(TRUE);

  JOIN_TAB *sjm_nest_end= NULL;
  JOIN_TAB *sjm_nest_root= NULL;

  for (j=join_tab, tablenr=0 ; tablenr < table_count ; tablenr++,j++)
  {
    TABLE *form;
    POSITION *cur_pos= &best_positions[tablenr];
    if (cur_pos->sj_strategy == SJ_OPT_MATERIALIZE || 
        cur_pos->sj_strategy == SJ_OPT_MATERIALIZE_SCAN)
    {
      /*
        Ok, we've entered an SJ-Materialization semi-join (note that this can't
        be done recursively, semi-joins are not allowed to be nested).
        1. Put into main join order a JOIN_TAB that represents a lookup or scan
           in the temptable.
      */
      bzero((void*)j, sizeof(JOIN_TAB));
      j->join= this;
      j->table= NULL; //temporary way to tell SJM tables from others.
      j->ref.key = -1;
      j->on_expr_ref= (Item**) &null_ptr;
      j->keys= key_map(1); /* The unique index is always in 'possible keys' in EXPLAIN */

      /*
        2. Proceed with processing SJM nest's join tabs, putting them into the
           sub-order
      */
      SJ_MATERIALIZATION_INFO *sjm= cur_pos->table->emb_sj_nest->sj_mat_info;
      j->records_read= (sjm->is_sj_scan? sjm->rows : 1);
      j->records= (ha_rows) j->records_read;
      j->cond_selectivity= 1.0;
      JOIN_TAB *jt;
      JOIN_TAB_RANGE *jt_range;
      if (!(jt= (JOIN_TAB*) thd->alloc(sizeof(JOIN_TAB)*sjm->tables)) ||
          !(jt_range= new JOIN_TAB_RANGE))
        DBUG_RETURN(TRUE);
      jt_range->start= jt;
      jt_range->end= jt + sjm->tables;
      join_tab_ranges.push_back(jt_range, thd->mem_root);
      j->bush_children= jt_range;
      sjm_nest_end= jt + sjm->tables;
      sjm_nest_root= j;

      j= jt;
    }
    
    *j= *best_positions[tablenr].table;

    j->bush_root_tab= sjm_nest_root;

    form= table[tablenr]= j->table;
    form->reginfo.join_tab=j;
    DBUG_PRINT("info",("type: %d", j->type));
    if (j->type == JT_CONST)
      goto loop_end;					// Handled in make_join_stat..

    j->loosescan_match_tab= NULL;  //non-nulls will be set later
    j->inside_loosescan_range= FALSE;
    j->ref.key = -1;
    j->ref.key_parts=0;

    if (j->type == JT_SYSTEM)
      goto loop_end;
    if ( !(keyuse= best_positions[tablenr].key))
    {
      j->type=JT_ALL;
      if (best_positions[tablenr].use_join_buffer &&
          tablenr != const_tables)
	full_join= 1;
    }

    /*if (best_positions[tablenr].sj_strategy == SJ_OPT_LOOSE_SCAN)
    {
      DBUG_ASSERT(!keyuse || keyuse->key ==
                             best_positions[tablenr].loosescan_picker.loosescan_key);
      j->index= best_positions[tablenr].loosescan_picker.loosescan_key;
    }*/

    if ((j->type == JT_REF || j->type == JT_EQ_REF) &&
        is_hash_join_key_no(j->ref.key))
      hash_join= TRUE; 

  loop_end:
    /* 
      Save records_read in JOIN_TAB so that select_describe()/etc don't have
      to access join->best_positions[]. 
    */
    j->records_read= best_positions[tablenr].records_read;
    j->cond_selectivity= best_positions[tablenr].cond_selectivity;
    map2table[j->table->tablenr]= j;

    /* If we've reached the end of sjm nest, switch back to main sequence */
    if (j + 1 == sjm_nest_end)
    {
      j->last_leaf_in_bush= TRUE;
      j= sjm_nest_root;
      sjm_nest_root= NULL;
      sjm_nest_end= NULL;
    }
  }
  root_range->end= j;

  used_tables= OUTER_REF_TABLE_BIT;		// Outer row is already read
  for (j=join_tab, tablenr=0 ; tablenr < table_count ; tablenr++,j++)
  {
    if (j->bush_children)
      j= j->bush_children->start;

    used_tables|= j->table->map;
    if (j->type != JT_CONST && j->type != JT_SYSTEM)
    {
      if ((keyuse= best_positions[tablenr].key) &&
          create_ref_for_key(this, j, keyuse, TRUE, used_tables))
        DBUG_RETURN(TRUE);              // Something went wrong
    }
    if (j->last_leaf_in_bush)
      j= j->bush_root_tab;
  }
 
  top_join_tab_count= (uint)(join_tab_ranges.head()->end - 
                      join_tab_ranges.head()->start);

  update_depend_map(this);
  DBUG_RETURN(0);
}