make_join_select(JOIN *join,SQL_SELECT *select,COND *cond)
{
  THD *thd= join->thd;
  DBUG_ENTER("make_join_select");
  if (select)
  {
    add_not_null_conds(join);
    table_map used_tables;
    /*
      Step #1: Extract constant condition
       - Extract and check the constant part of the WHERE 
       - Extract constant parts of ON expressions from outer 
         joins and attach them appropriately.
    */
    if (cond)                /* Because of QUICK_GROUP_MIN_MAX_SELECT */
    {                        /* there may be a select without a cond. */    
      if (join->table_count > 1)
        cond->update_used_tables();		// Tablenr may have changed

      /*
        Extract expressions that depend on constant tables
        1. Const part of the join's WHERE clause can be checked immediately
           and if it is not satisfied then the join has empty result
        2. Constant parts of outer joins' ON expressions must be attached 
           there inside the triggers.
      */
      {						// Check const tables
        join->exec_const_cond=
	  make_cond_for_table(thd, cond,
                              join->const_table_map,
                              (table_map) 0, -1, FALSE, FALSE);
        /* Add conditions added by add_not_null_conds(). */
        for (uint i= 0 ; i < join->const_tables ; i++)
          add_cond_and_fix(thd, &join->exec_const_cond,
                           join->join_tab[i].select_cond);

        DBUG_EXECUTE("where",print_where(join->exec_const_cond,"constants",
					 QT_ORDINARY););
        if (join->exec_const_cond && !join->exec_const_cond->is_expensive() &&
            !join->exec_const_cond->val_int())
        {
          DBUG_PRINT("info",("Found impossible WHERE condition"));
          join->exec_const_cond= NULL;
          DBUG_RETURN(1);	 // Impossible const condition
        }

        if (join->table_count != join->const_tables)
        {
          COND *outer_ref_cond= make_cond_for_table(thd, cond,
                                                    join->const_table_map |
                                                    OUTER_REF_TABLE_BIT,
                                                    OUTER_REF_TABLE_BIT,
                                                    -1, FALSE, FALSE);
          if (outer_ref_cond)
          {
            add_cond_and_fix(thd, &outer_ref_cond, join->outer_ref_cond);
            join->outer_ref_cond= outer_ref_cond;
          }
        }
        else
        {
          COND *pseudo_bits_cond=
            make_cond_for_table(thd, cond,
                                join->const_table_map |
                                PSEUDO_TABLE_BITS,
                                PSEUDO_TABLE_BITS,
                                -1, FALSE, FALSE);
          if (pseudo_bits_cond)
          {
            add_cond_and_fix(thd, &pseudo_bits_cond,
                             join->pseudo_bits_cond);
            join->pseudo_bits_cond= pseudo_bits_cond;
          }
        }
      }
    }

    /*
      Step #2: Extract WHERE/ON parts
    */
    uint i;
    for (i= join->top_join_tab_count - 1; i >= join->const_tables; i--)
    {
      if (!join->join_tab[i].bush_children)
        break;
    }
    uint last_top_base_tab_idx= i;

    table_map save_used_tables= 0;
    used_tables=((select->const_tables=join->const_table_map) |
		 OUTER_REF_TABLE_BIT | RAND_TABLE_BIT);
    JOIN_TAB *tab;
    table_map current_map;
    i= join->const_tables;
    for (tab= first_depth_first_tab(join); tab;
         tab= next_depth_first_tab(join, tab))
    {
      bool is_hj;

      /*
        first_inner is the X in queries like:
        SELECT * FROM t1 LEFT OUTER JOIN (t2 JOIN t3) ON X
      */
      JOIN_TAB *first_inner_tab= tab->first_inner;

      if (!tab->bush_children)
        current_map= tab->table->map;
      else
        current_map= tab->bush_children->start->emb_sj_nest->sj_inner_tables;

      bool use_quick_range=0;
      COND *tmp;

      /* 
        Tables that are within SJ-Materialization nests cannot have their
        conditions referring to preceding non-const tables.
         - If we're looking at the first SJM table, reset used_tables
           to refer to only allowed tables
      */
      if (tab->emb_sj_nest && tab->emb_sj_nest->sj_mat_info && 
          tab->emb_sj_nest->sj_mat_info->is_used &&
          !(used_tables & tab->emb_sj_nest->sj_inner_tables))
      {
        save_used_tables= used_tables;
        used_tables= join->const_table_map | OUTER_REF_TABLE_BIT | 
                     RAND_TABLE_BIT;
      }

      used_tables|=current_map;

      if (tab->type == JT_REF && tab->quick &&
	  (((uint) tab->ref.key == tab->quick->index &&
	    tab->ref.key_length < tab->quick->max_used_key_length) ||
           (!is_hash_join_key_no(tab->ref.key) &&
            tab->table->intersect_keys.is_set(tab->ref.key))))
      {
	/* Range uses longer key;  Use this instead of ref on key */
	tab->type=JT_ALL;
	use_quick_range=1;
	tab->use_quick=1;
        tab->ref.key= -1;
	tab->ref.key_parts=0;		// Don't use ref key.
	join->best_positions[i].records_read= rows2double(tab->quick->records);
        /* 
          We will use join cache here : prevent sorting of the first
          table only and sort at the end.
        */
        if (i != join->const_tables &&
            join->table_count > join->const_tables + 1 &&
            join->best_positions[i].use_join_buffer)
          join->full_join= 1;
      }

      tmp= NULL;

      if (cond)
      {
        if (tab->bush_children)
        {
          // Reached the materialization tab
          tmp= make_cond_after_sjm(thd, cond, cond, save_used_tables,
                                   used_tables, /*inside_or_clause=*/FALSE);
          used_tables= save_used_tables | used_tables;
          save_used_tables= 0;
        }
        else
        {
          tmp= make_cond_for_table(thd, cond, used_tables, current_map, i,
                                   FALSE, FALSE);
          if (tab == join->join_tab + last_top_base_tab_idx)
          {
            /*
              This pushes conjunctive conditions of WHERE condition such that:
              - their used_tables() contain RAND_TABLE_BIT
              - the conditions does not refer to any fields
              (such like rand() > 0.5)
            */
            table_map rand_table_bit= (table_map) RAND_TABLE_BIT;
            COND *rand_cond= make_cond_for_table(thd, cond, used_tables,
                                                 rand_table_bit, -1,
                                                 FALSE, FALSE);
            add_cond_and_fix(thd, &tmp, rand_cond);
          }
        }
        /* Add conditions added by add_not_null_conds(). */
        if (tab->select_cond)
          add_cond_and_fix(thd, &tmp, tab->select_cond);
      }

      is_hj= (tab->type == JT_REF || tab->type == JT_EQ_REF) &&
             (join->allowed_join_cache_types & JOIN_CACHE_HASHED_BIT) &&
	     ((join->max_allowed_join_cache_level+1)/2 == 2 ||
              ((join->max_allowed_join_cache_level+1)/2 > 2 &&
	       is_hash_join_key_no(tab->ref.key))) &&
              (!tab->emb_sj_nest ||                     
               join->allowed_semijoin_with_cache) && 
              (!(tab->table->map & join->outer_join) ||
               join->allowed_outer_join_with_cache);

      if (cond && !tmp && tab->quick)
      {						// Outer join
        if (tab->type != JT_ALL && !is_hj)
        {
          /*
            Don't use the quick method
            We come here in the case where we have 'key=constant' and
            the test is removed by make_cond_for_table()
          */
          delete tab->quick;
          tab->quick= 0;
        }
        else
        {
          /*
            Hack to handle the case where we only refer to a table
            in the ON part of an OUTER JOIN. In this case we want the code
            below to check if we should use 'quick' instead.
          */
          DBUG_PRINT("info", ("Item_int"));
          tmp= new (thd->mem_root) Item_int(thd, (longlong) 1, 1); // Always true
        }

      }
      if (tmp || !cond || tab->type == JT_REF || tab->type == JT_REF_OR_NULL ||
          tab->type == JT_EQ_REF || first_inner_tab)
      {
        DBUG_EXECUTE("where",print_where(tmp, 
                                         tab->table? tab->table->alias.c_ptr() :"sjm-nest",
                                         QT_ORDINARY););
	SQL_SELECT *sel= tab->select= ((SQL_SELECT*)
                                       thd->memdup((uchar*) select,
                                                   sizeof(*select)));
	if (!sel)
	  DBUG_RETURN(1);			// End of memory
        /*
          If tab is an inner table of an outer join operation,
          add a match guard to the pushed down predicate.
          The guard will turn the predicate on only after
          the first match for outer tables is encountered.
	*/        
        if (cond && tmp)
        {
          /*
            Because of QUICK_GROUP_MIN_MAX_SELECT there may be a select without
            a cond, so neutralize the hack above.
          */
          COND *tmp_cond;
          if (!(tmp_cond= add_found_match_trig_cond(thd, first_inner_tab, tmp,
                                                    0)))
            DBUG_RETURN(1);
          sel->cond= tmp_cond;
          tab->set_select_cond(tmp_cond, __LINE__);
          /* Push condition to storage engine if this is enabled
             and the condition is not guarded */
          if (tab->table)
          {
            tab->table->file->pushed_cond= NULL;
            if ((tab->table->file->ha_table_flags() &
                  HA_CAN_TABLE_CONDITION_PUSHDOWN) &&
                !first_inner_tab)
            {
              COND *push_cond= 
              make_cond_for_table(thd, tmp_cond, current_map, current_map,
                                  -1, FALSE, FALSE);
              if (push_cond)
              {
                /* Push condition to handler */
                if (!tab->table->file->cond_push(push_cond))
                  tab->table->file->pushed_cond= push_cond;
              }
            }
          }
        }
        else
        {
          sel->cond= NULL;
          tab->set_select_cond(NULL, __LINE__);
        }

	sel->head=tab->table;
        DBUG_EXECUTE("where",
                     print_where(tmp, 
                                 tab->table ? tab->table->alias.c_ptr() :
                                   "(sjm-nest)",
                                 QT_ORDINARY););
	if (tab->quick)
	{
	  /* Use quick key read if it's a constant and it's not used
	     with key reading */
          if ((tab->needed_reg.is_clear_all() && tab->type != JT_EQ_REF &&
              tab->type != JT_FT &&
              ((tab->type != JT_CONST && tab->type != JT_REF) ||
               (uint) tab->ref.key == tab->quick->index)) || is_hj)
          {
            DBUG_ASSERT(tab->quick->is_valid());
	    sel->quick=tab->quick;		// Use value from get_quick_...
	    sel->quick_keys.clear_all();
	    sel->needed_reg.clear_all();
	  }
	  else
	  {
	    delete tab->quick;
	  }
	  tab->quick=0;
	}
	uint ref_key= sel->head? (uint) sel->head->reginfo.join_tab->ref.key+1 : 0;
	if (i == join->const_tables && ref_key)
	{
	  if (!tab->const_keys.is_clear_all() &&
              tab->table->reginfo.impossible_range)
	    DBUG_RETURN(1);
	}
	else if (tab->type == JT_ALL && ! use_quick_range)
	{
	  if (!tab->const_keys.is_clear_all() &&
	      tab->table->reginfo.impossible_range)
	    DBUG_RETURN(1);				// Impossible range
	  /*
	    We plan to scan all rows.
	    Check again if we should use an index.

            There are two cases:
            1) There could be an index usage the refers to a previous
               table that we didn't consider before, but could be consider
               now as a "last resort". For example
               SELECT * from t1,t2 where t1.a between t2.a and t2.b;
            2) If the current table is the first non const table
               and there is a limit it still possibly beneficial
               to use the index even if the index range is big as
               we can stop when we've found limit rows.

            (1) - Don't switch the used index if we are using semi-join
                  LooseScan on this table. Using different index will not
                  produce the desired ordering and de-duplication.
	  */

	  if (!tab->table->is_filled_at_execution() &&
              !tab->loosescan_match_tab &&              // (1)
              ((cond && (!tab->keys.is_subset(tab->const_keys) && i > 0)) ||
               (!tab->const_keys.is_clear_all() && i == join->const_tables &&
                join->unit->select_limit_cnt <
                join->best_positions[i].records_read &&
                !(join->select_options & OPTION_FOUND_ROWS))))
	  {
	    /* Join with outer join condition */
	    COND *orig_cond=sel->cond;
	    sel->cond= and_conds(thd, sel->cond, *tab->on_expr_ref);

	    /*
              We can't call sel->cond->fix_fields,
              as it will break tab->on_expr if it's AND condition
              (fix_fields currently removes extra AND/OR levels).
              Yet attributes of the just built condition are not needed.
              Thus we call sel->cond->quick_fix_field for safety.
	    */
	    if (sel->cond && !sel->cond->fixed)
	      sel->cond->quick_fix_field();

	    if (sel->test_quick_select(thd, tab->keys,
				       ((used_tables & ~ current_map) |
                                        OUTER_REF_TABLE_BIT),
				       (join->select_options &
					OPTION_FOUND_ROWS ?
					HA_POS_ERROR :
					join->unit->select_limit_cnt), 0,
                                        FALSE, FALSE) < 0)
            {
	      /*
		Before reporting "Impossible WHERE" for the whole query
		we have to check isn't it only "impossible ON" instead
	      */
              sel->cond=orig_cond;
              if (!*tab->on_expr_ref ||
                  sel->test_quick_select(thd, tab->keys,
                                         used_tables & ~ current_map,
                                         (join->select_options &
                                          OPTION_FOUND_ROWS ?
                                          HA_POS_ERROR :
                                          join->unit->select_limit_cnt),0,
                                          FALSE, FALSE) < 0)
		DBUG_RETURN(1);			// Impossible WHERE
            }
            else
	      sel->cond=orig_cond;

	    /* Fix for EXPLAIN */
	    if (sel->quick)
	      join->best_positions[i].records_read= (double)sel->quick->records;
	  }
	  else
	  {
	    sel->needed_reg=tab->needed_reg;
	  }
	  sel->quick_keys= tab->table->quick_keys;
	  if (!sel->quick_keys.is_subset(tab->checked_keys) ||
              !sel->needed_reg.is_subset(tab->checked_keys))
	  {
            /*
              "Range checked for each record" is a "last resort" access method
              that should only be used when the other option is a cross-product
              join.

              We use the following condition (it's approximate):
              1. There are potential keys for (sel->needed_reg)
              2. There were no possible ways to construct a quick select, or
                 the quick select would be more expensive than the full table
                 scan.
            */
	    tab->use_quick= (!sel->needed_reg.is_clear_all() &&
			     (sel->quick_keys.is_clear_all() ||
                              (sel->quick && 
                               sel->quick->read_time > 
                               tab->table->file->scan_time() + 
                               tab->table->file->stats.records/TIME_FOR_COMPARE
                               ))) ?
	      2 : 1;
	    sel->read_tables= used_tables & ~current_map;
            sel->quick_keys.clear_all();
	  }
	  if (i != join->const_tables && tab->use_quick != 2 &&
              !tab->first_inner)
	  {					/* Read with cache */
            if (tab->make_scan_filter())
              DBUG_RETURN(1);
          }
	}
      }
      
      /* 
        Push down conditions from all ON expressions.
        Each of these conditions are guarded by a variable
        that turns if off just before null complemented row for
        outer joins is formed. Thus, the condition from an
        'on expression' are guaranteed not to be checked for
        the null complemented row.
      */ 

      /* 
        First push down constant conditions from ON expressions. 
         - Each pushed-down condition is wrapped into trigger which is 
           enabled only for non-NULL-complemented record
         - The condition is attached to the first_inner_table.
        
        With regards to join nests:
         - if we start at top level, don't walk into nests
         - if we start inside a nest, stay within that nest.
      */
      JOIN_TAB *start_from= tab->bush_root_tab? 
                               tab->bush_root_tab->bush_children->start : 
                               join->join_tab + join->const_tables;
      JOIN_TAB *end_with= tab->bush_root_tab? 
                               tab->bush_root_tab->bush_children->end : 
                               join->join_tab + join->top_join_tab_count;
      for (JOIN_TAB *join_tab= start_from;
           join_tab != end_with;
           join_tab++)
      {
        if (*join_tab->on_expr_ref)
        {
          JOIN_TAB *cond_tab= join_tab->first_inner;
          COND *tmp_cond= make_cond_for_table(thd, *join_tab->on_expr_ref,
                                              join->const_table_map,
                                              (table_map) 0, -1, FALSE, FALSE);
          if (!tmp_cond)
            continue;
          tmp_cond= new (thd->mem_root) Item_func_trig_cond(thd, tmp_cond,
                                            &cond_tab->not_null_compl);
          if (!tmp_cond)
            DBUG_RETURN(1);
          tmp_cond->quick_fix_field();
          cond_tab->select_cond= !cond_tab->select_cond ? tmp_cond :
                                 new (thd->mem_root) Item_cond_and(thd, cond_tab->select_cond,
                                                   tmp_cond);
          if (!cond_tab->select_cond)
	    DBUG_RETURN(1);
          cond_tab->select_cond->quick_fix_field();
          cond_tab->select_cond->update_used_tables();
          if (cond_tab->select)
            cond_tab->select->cond= cond_tab->select_cond; 
        }       
      }


      /* Push down non-constant conditions from ON expressions */
      JOIN_TAB *last_tab= tab;

      /*
        while we're inside of an outer join and last_tab is 
        the last of its tables ... 
      */
      while (first_inner_tab && first_inner_tab->last_inner == last_tab)
      { 
        /* 
          Table tab is the last inner table of an outer join.
          An on expression is always attached to it.
	*/     
        COND *on_expr= *first_inner_tab->on_expr_ref;

        table_map used_tables2= (join->const_table_map |
                                 OUTER_REF_TABLE_BIT | RAND_TABLE_BIT);

        start_from= tab->bush_root_tab? 
                      tab->bush_root_tab->bush_children->start : 
                      join->join_tab + join->const_tables;
        for (JOIN_TAB *inner_tab= start_from;
             inner_tab <= last_tab;
             inner_tab++)
        {
          DBUG_ASSERT(inner_tab->table);
          current_map= inner_tab->table->map;
          used_tables2|= current_map;
          /*
            psergey: have put the -1 below. It's bad, will need to fix it.
          */
          COND *tmp_cond= make_cond_for_table(thd, on_expr, used_tables2,
                                              current_map,
                                              /*(inner_tab - first_tab)*/ -1,
					      FALSE, FALSE);
          if (tab == last_tab)
          {
            /*
              This pushes conjunctive conditions of ON expression of an outer
              join such that:
              - their used_tables() contain RAND_TABLE_BIT
              - the conditions does not refer to any fields
              (such like rand() > 0.5)
            */
            table_map rand_table_bit= (table_map) RAND_TABLE_BIT;
            COND *rand_cond= make_cond_for_table(thd, on_expr, used_tables2,
                                                 rand_table_bit, -1,
                                                 FALSE, FALSE);
            add_cond_and_fix(thd, &tmp_cond, rand_cond);
          }
          bool is_sjm_lookup_tab= FALSE;
          if (inner_tab->bush_children)
          {
            /*
              'inner_tab' is an SJ-Materialization tab, i.e. we have a join
              order like this:

                ot1 sjm_tab LEFT JOIN ot2 ot3
                         ^          ^
                   'tab'-+          +--- left join we're adding triggers for

              LEFT JOIN's ON expression may not have references to subquery
              columns.  The subquery was in the WHERE clause, so IN-equality 
              is in the WHERE clause, also.
              However, equality propagation code may have propagated the
              IN-equality into ON expression, and we may get things like

                subquery_inner_table=const

              in the ON expression. We must not check such conditions during
              SJM-lookup, because 1) subquery_inner_table has no valid current
              row (materialization temp.table has it instead), and 2) they
              would be true anyway.
            */
            SJ_MATERIALIZATION_INFO *sjm=
              inner_tab->bush_children->start->emb_sj_nest->sj_mat_info;
            if (sjm->is_used && !sjm->is_sj_scan)
              is_sjm_lookup_tab= TRUE;
          }

          if (inner_tab == first_inner_tab && inner_tab->on_precond &&
              !is_sjm_lookup_tab)
            add_cond_and_fix(thd, &tmp_cond, inner_tab->on_precond);
          if (tmp_cond && !is_sjm_lookup_tab)
          {
            JOIN_TAB *cond_tab=  (inner_tab < first_inner_tab ?
                                  first_inner_tab : inner_tab);
            Item **sel_cond_ref= (inner_tab < first_inner_tab ?
                                  &first_inner_tab->on_precond :
                                  &inner_tab->select_cond);
            /*
              First add the guards for match variables of
              all embedding outer join operations.
	    */
            if (!(tmp_cond= add_found_match_trig_cond(thd,
                                                     cond_tab->first_inner,
                                                     tmp_cond,
                                                     first_inner_tab)))
              DBUG_RETURN(1);
            /* 
              Now add the guard turning the predicate off for 
              the null complemented row.
	    */ 
            DBUG_PRINT("info", ("Item_func_trig_cond"));
            tmp_cond= new (thd->mem_root) Item_func_trig_cond(thd, tmp_cond,
                                              &first_inner_tab->
                                              not_null_compl);
            DBUG_PRINT("info", ("Item_func_trig_cond %p",
                                tmp_cond));
            if (tmp_cond)
              tmp_cond->quick_fix_field();
	    /* Add the predicate to other pushed down predicates */
            DBUG_PRINT("info", ("Item_cond_and"));
            *sel_cond_ref= !(*sel_cond_ref) ? 
                             tmp_cond :
                             new (thd->mem_root) Item_cond_and(thd, *sel_cond_ref, tmp_cond);
            DBUG_PRINT("info", ("Item_cond_and %p",
                                (*sel_cond_ref)));
            if (!(*sel_cond_ref))
              DBUG_RETURN(1);
            (*sel_cond_ref)->quick_fix_field();
            (*sel_cond_ref)->update_used_tables();
            if (cond_tab->select)
              cond_tab->select->cond= cond_tab->select_cond;
          }
        }
        first_inner_tab= first_inner_tab->first_upper;       
      }
      if (!tab->bush_children)
        i++;
    }
  }
  DBUG_RETURN(0);
}