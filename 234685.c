make_join_readinfo(JOIN *join, ulonglong options, uint no_jbuf_after)
{
  JOIN_TAB *tab;
  uint i;
  DBUG_ENTER("make_join_readinfo");

  bool statistics= MY_TEST(!(join->select_options & SELECT_DESCRIBE));
  bool sorted= 1;

  join->complex_firstmatch_tables= table_map(0);

  if (!join->select_lex->sj_nests.is_empty() &&
      setup_semijoin_dups_elimination(join, options, no_jbuf_after))
    DBUG_RETURN(TRUE); /* purecov: inspected */
  
  /* For const tables, set partial_join_cardinality to 1. */
  for (tab= join->join_tab; tab != join->join_tab + join->const_tables; tab++)
    tab->partial_join_cardinality= 1; 

  JOIN_TAB *prev_tab= NULL;
  i= join->const_tables;
  for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       prev_tab=tab, tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
    /*
      The approximation below for partial join cardinality is not good because
        - it does not take into account some pushdown predicates
        - it does not differentiate between inner joins, outer joins and
        semi-joins.
      Later it should be improved.
    */

    if (tab->bush_root_tab && tab->bush_root_tab->bush_children->start == tab)
      prev_tab= NULL;
    DBUG_ASSERT(tab->bush_children || tab->table == join->best_positions[i].table->table);

    tab->partial_join_cardinality= join->best_positions[i].records_read *
                                   (prev_tab? prev_tab->partial_join_cardinality : 1);
    if (!tab->bush_children)
      i++;
  }
 
  check_join_cache_usage_for_tables(join, options, no_jbuf_after);
  
  JOIN_TAB *first_tab;
  for (tab= first_tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
    if (tab->bush_children)
    {
      if (setup_sj_materialization_part2(tab))
        return TRUE;
    }

    TABLE *table=tab->table;
    uint jcl= tab->used_join_cache_level;
    tab->read_record.table= table;
    tab->read_record.unlock_row= rr_unlock_row;
    tab->sorted= sorted;
    sorted= 0;                                  // only first must be sorted
    

    /*
      We should not set tab->next_select for the last table in the
      SMJ-nest, as setup_sj_materialization() has already set it to
      end_sj_materialize.
    */
    if (!(tab->bush_root_tab && 
          tab->bush_root_tab->bush_children->end == tab + 1))
    {
      tab->next_select=sub_select;		/* normal select */
    }


    if (tab->loosescan_match_tab)
    {
      if (!(tab->loosescan_buf= (uchar*)join->thd->alloc(tab->
                                                         loosescan_key_len)))
        return TRUE; /* purecov: inspected */
      tab->sorted= TRUE;
    }
    table->status=STATUS_NO_RECORD;
    pick_table_access_method (tab);

    if (jcl)
       tab[-1].next_select=sub_select_cache;

    if (tab->cache && tab->cache->get_join_alg() == JOIN_CACHE::BNLH_JOIN_ALG)
      tab->type= JT_HASH;
      
    switch (tab->type) {
    case JT_SYSTEM:				// Only happens with left join 
    case JT_CONST:				// Only happens with left join
      /* Only happens with outer joins */
      tab->read_first_record= tab->type == JT_SYSTEM ? join_read_system
                                                     : join_read_const;
      if (table->covering_keys.is_set(tab->ref.key) && !table->no_keyread)
        table->file->ha_start_keyread(tab->ref.key);
      else if ((!jcl || jcl > 4) && !tab->ref.is_access_triggered())
        push_index_cond(tab, tab->ref.key);
      break;
    case JT_EQ_REF:
      tab->read_record.unlock_row= join_read_key_unlock_row;
      /* fall through */
      if (table->covering_keys.is_set(tab->ref.key) && !table->no_keyread)
        table->file->ha_start_keyread(tab->ref.key);
      else if ((!jcl || jcl > 4) && !tab->ref.is_access_triggered())
        push_index_cond(tab, tab->ref.key);
      break;
    case JT_REF_OR_NULL:
    case JT_REF:
      if (tab->select)
      {
	delete tab->select->quick;
	tab->select->quick=0;
      }
      delete tab->quick;
      tab->quick=0;
      if (table->covering_keys.is_set(tab->ref.key) && !table->no_keyread)
        table->file->ha_start_keyread(tab->ref.key);
      else if ((!jcl || jcl > 4) && !tab->ref.is_access_triggered())
        push_index_cond(tab, tab->ref.key);
      break;
    case JT_ALL:
    case JT_HASH:
      /*
	If previous table use cache
        If the incoming data set is already sorted don't use cache.
        Also don't use cache if this is the first table in semi-join
          materialization nest.
      */
      /* These init changes read_record */
      if (tab->use_quick == 2)
      {
        join->thd->set_status_no_good_index_used();
	tab->read_first_record= join_init_quick_read_record;
	if (statistics)
	  join->thd->inc_status_select_range_check();
      }
      else
      {
        if (!tab->bush_children)
          tab->read_first_record= join_init_read_record;
	if (tab == first_tab)
	{
	  if (tab->select && tab->select->quick)
	  {
	    if (statistics)
	      join->thd->inc_status_select_range();
	  }
	  else
	  {
            join->thd->set_status_no_index_used();
	    if (statistics)
	    {
              join->thd->inc_status_select_scan();
	      join->thd->query_plan_flags|= QPLAN_FULL_SCAN;
	    }
	  }
	}
	else
	{
	  if (tab->select && tab->select->quick)
	  {
	    if (statistics)
              join->thd->inc_status_select_full_range_join();
	  }
	  else
	  {
            join->thd->set_status_no_index_used();
	    if (statistics)
	    {
              join->thd->inc_status_select_full_join();
	      join->thd->query_plan_flags|= QPLAN_FULL_JOIN;
	    }
	  }
	}
	if (!table->no_keyread)
	{
	  if (tab->select && tab->select->quick &&
              tab->select->quick->index != MAX_KEY && //not index_merge
	      table->covering_keys.is_set(tab->select->quick->index))
            table->file->ha_start_keyread(tab->select->quick->index);
	  else if (!table->covering_keys.is_clear_all() &&
		   !(tab->select && tab->select->quick))
	  {					// Only read index tree
            if (tab->loosescan_match_tab)
              tab->index= tab->loosescan_key;
            else 
            {
#ifdef BAD_OPTIMIZATION
              /*
                It has turned out that the below change, while speeding things
                up for disk-bound loads, slows them down for cases when the data
                is in disk cache (see BUG#35850):
                See bug #26447: "Using the clustered index for a table scan
                is always faster than using a secondary index".
              */
              if (table->s->primary_key != MAX_KEY &&
                  table->file->primary_key_is_clustered())
                tab->index= table->s->primary_key;
              else
#endif
                tab->index=find_shortest_key(table, & table->covering_keys);
            }
	    tab->read_first_record= join_read_first;
            /* Read with index_first / index_next */
	    tab->type= tab->type == JT_ALL ? JT_NEXT : JT_HASH_NEXT;		
	  }
	}
        if (tab->select && tab->select->quick &&
            tab->select->quick->index != MAX_KEY &&
            !tab->table->file->keyread_enabled())
          push_index_cond(tab, tab->select->quick->index);
      }
      break;
    case JT_FT:
      break;
      /* purecov: begin deadcode */
    default:
      DBUG_PRINT("error",("Table type %d found",tab->type));
      break;
    case JT_UNKNOWN:
    case JT_MAYBE_REF:
      abort();
      /* purecov: end */
    }

    DBUG_EXECUTE("where",
                 char buff[256];
                 String str(buff,sizeof(buff),system_charset_info);
                 str.length(0);
                 str.append(tab->table? tab->table->alias.c_ptr() :"<no_table_name>");
                 str.append(" final_pushdown_cond");
                 print_where(tab->select_cond, str.c_ptr_safe(), QT_ORDINARY););
  }
  uint n_top_tables= (uint)(join->join_tab_ranges.head()->end -  
                     join->join_tab_ranges.head()->start);

  join->join_tab[n_top_tables - 1].next_select=0;  /* Set by do_select */
  
  /*
    If a join buffer is used to join a table the ordering by an index
    for the first non-constant table cannot be employed anymore.
  */
  for (tab= join->join_tab + join->const_tables ; 
       tab != join->join_tab + n_top_tables ; tab++)
  {
    if (tab->use_join_cache)
    {
       JOIN_TAB *sort_by_tab= join->group && join->simple_group &&
                              join->group_list ?
			       join->join_tab+join->const_tables :
                               join->get_sort_by_join_tab();
      /*
        It could be that sort_by_tab==NULL, and the plan is to use filesort()
        on the first table.
      */
      if (join->order)
      {
        join->simple_order= 0;
        join->need_tmp= 1;
      }

      if (join->group && !join->group_optimized_away)
      {
        join->need_tmp= 1;
        join->simple_group= 0;
      }
      
      if (sort_by_tab)
      {
        join->need_tmp= 1;
        join->simple_order= join->simple_group= 0;
        if (sort_by_tab->type == JT_NEXT && 
            !sort_by_tab->table->covering_keys.is_set(sort_by_tab->index))
        {
          sort_by_tab->type= JT_ALL;
          sort_by_tab->read_first_record= join_init_read_record;
        }
        else if (sort_by_tab->type == JT_HASH_NEXT &&
                 !sort_by_tab->table->covering_keys.is_set(sort_by_tab->index))
        {
          sort_by_tab->type= JT_HASH;
          sort_by_tab->read_first_record= join_init_read_record;
        }
      }
      break;
    }
  }

  DBUG_RETURN(FALSE);
}