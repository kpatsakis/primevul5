int JOIN::optimize_stage2()
{
  ulonglong select_opts_for_readinfo;
  uint no_jbuf_after;
  JOIN_TAB *tab;
  DBUG_ENTER("JOIN::optimize_stage2");

  if (subq_exit_fl)
    goto setup_subq_exit;

  if (unlikely(thd->check_killed()))
    DBUG_RETURN(1);

  /* Generate an execution plan from the found optimal join order. */
  if (get_best_combination())
    DBUG_RETURN(1);

  if (select_lex->handle_derived(thd->lex, DT_OPTIMIZE))
    DBUG_RETURN(1);

  if (optimizer_flag(thd, OPTIMIZER_SWITCH_DERIVED_WITH_KEYS))
    drop_unused_derived_keys();

  if (rollup.state != ROLLUP::STATE_NONE)
  {
    if (rollup_process_const_fields())
    {
      DBUG_PRINT("error", ("Error: rollup_process_fields() failed"));
      DBUG_RETURN(1);
    }
  }
  else
  {
    /* Remove distinct if only const tables */
    select_distinct= select_distinct && (const_tables != table_count);
  }

  THD_STAGE_INFO(thd, stage_preparing);
  if (result->initialize_tables(this))
  {
    DBUG_PRINT("error",("Error: initialize_tables() failed"));
    DBUG_RETURN(1);				// error == -1
  }
  if (const_table_map != found_const_table_map &&
      !(select_options & SELECT_DESCRIBE))
  {
    // There is at least one empty const table
    zero_result_cause= "no matching row in const table";
    DBUG_PRINT("error",("Error: %s", zero_result_cause));
    error= 0;
    handle_implicit_grouping_with_window_funcs();
    goto setup_subq_exit;
  }
  if (!(thd->variables.option_bits & OPTION_BIG_SELECTS) &&
      best_read > (double) thd->variables.max_join_size &&
      !(select_options & SELECT_DESCRIBE))
  {						/* purecov: inspected */
    my_message(ER_TOO_BIG_SELECT, ER_THD(thd, ER_TOO_BIG_SELECT), MYF(0));
    error= -1;
    DBUG_RETURN(1);
  }
  if (const_tables && !thd->locked_tables_mode &&
      !(select_options & SELECT_NO_UNLOCK))
  {
    /*
      Unlock all tables, except sequences, as accessing these may still
      require table updates
    */
    mysql_unlock_some_tables(thd, table, const_tables,
                             GET_LOCK_SKIP_SEQUENCES);
  }
  if (!conds && outer_join)
  {
    /* Handle the case where we have an OUTER JOIN without a WHERE */
    conds= new (thd->mem_root) Item_int(thd, (longlong) 1,1); // Always true
  }

  if (impossible_where)
  {
    zero_result_cause=
      "Impossible WHERE noticed after reading const tables";
    select_lex->mark_const_derived(zero_result_cause);
    handle_implicit_grouping_with_window_funcs();
    goto setup_subq_exit;
  }

  select= make_select(*table, const_table_map,
                      const_table_map, conds, (SORT_INFO*) 0, 1, &error);
  if (unlikely(error))
  {						/* purecov: inspected */
    error= -1;					/* purecov: inspected */
    DBUG_PRINT("error",("Error: make_select() failed"));
    DBUG_RETURN(1);
  }
  
  reset_nj_counters(this, join_list);
  if (make_outerjoin_info(this))
  {
    DBUG_RETURN(1);
  }

  /*
    Among the equal fields belonging to the same multiple equality
    choose the one that is to be retrieved first and substitute
    all references to these in where condition for a reference for
    the selected field.
  */
  if (conds)
  {
    conds= substitute_for_best_equal_field(thd, NO_PARTICULAR_TAB, conds,
                                           cond_equal, map2table);
    if (unlikely(thd->is_error()))
    {
      error= 1;
      DBUG_PRINT("error",("Error from substitute_for_best_equal"));
      DBUG_RETURN(1);
    }
    conds->update_used_tables();
    DBUG_EXECUTE("where",
                 print_where(conds,
                             "after substitute_best_equal",
                             QT_ORDINARY););
  }

  /*
    Perform the optimization on fields evaluation mentioned above
    for all on expressions.
  */
  for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES); tab;
       tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
  {
    if (*tab->on_expr_ref)
    {
      *tab->on_expr_ref= substitute_for_best_equal_field(thd, NO_PARTICULAR_TAB,
                                                         *tab->on_expr_ref,
                                                         tab->cond_equal,
                                                         map2table);
      if (unlikely(thd->is_error()))
      {
        error= 1;
        DBUG_PRINT("error",("Error from substitute_for_best_equal"));
        DBUG_RETURN(1);
      }
      (*tab->on_expr_ref)->update_used_tables();
    }
  }

  /*
    Perform the optimization on fields evaliation mentioned above
    for all used ref items.
  */
  for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES); tab;
       tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
  {
    uint key_copy_index=0;
    for (uint i=0; i < tab->ref.key_parts; i++)
    {
      Item **ref_item_ptr= tab->ref.items+i;
      Item *ref_item= *ref_item_ptr;
      if (!ref_item->used_tables() && !(select_options & SELECT_DESCRIBE))
        continue;
      COND_EQUAL *equals= cond_equal;
      JOIN_TAB *first_inner= tab->first_inner;
      while (equals)
      {
        ref_item= substitute_for_best_equal_field(thd, tab, ref_item,
                                                  equals, map2table);
        if (unlikely(thd->is_fatal_error))
          DBUG_RETURN(1);

        if (first_inner)
	{
          equals= first_inner->cond_equal;
          first_inner= first_inner->first_upper;
        }
        else
          equals= 0;
      }  
      ref_item->update_used_tables();
      if (*ref_item_ptr != ref_item)
      {
        *ref_item_ptr= ref_item;
        Item *item= ref_item->real_item();
        store_key *key_copy= tab->ref.key_copy[key_copy_index];
        if (key_copy->type() == store_key::FIELD_STORE_KEY)
        {
          if (item->basic_const_item())
          {
            /* It is constant propagated here */
            tab->ref.key_copy[key_copy_index]=
              new store_key_const_item(*tab->ref.key_copy[key_copy_index],
                                       item);
          }
          else if (item->const_item())
	  {
            tab->ref.key_copy[key_copy_index]=
              new store_key_item(*tab->ref.key_copy[key_copy_index],
                                 item, TRUE);
          }            
          else
          {
            store_key_field *field_copy= ((store_key_field *)key_copy);
            DBUG_ASSERT(item->type() == Item::FIELD_ITEM);
            field_copy->change_source_field((Item_field *) item);
          }
        }
      }
      key_copy_index++;
    }
  }

  if (conds && const_table_map != found_const_table_map &&
      (select_options & SELECT_DESCRIBE))
  {
    conds=new (thd->mem_root) Item_int(thd, (longlong) 0, 1); // Always false
  }

  /* Cache constant expressions in WHERE, HAVING, ON clauses. */
  cache_const_exprs();

  if (setup_semijoin_loosescan(this))
    DBUG_RETURN(1);

  if (make_join_select(this, select, conds))
  {
    zero_result_cause=
      "Impossible WHERE noticed after reading const tables";
    select_lex->mark_const_derived(zero_result_cause);
    handle_implicit_grouping_with_window_funcs();
    goto setup_subq_exit;
  }

  error= -1;					/* if goto err */

  /* Optimize distinct away if possible */
  {
    ORDER *org_order= order;
    order=remove_const(this, order,conds,1, &simple_order);
    if (unlikely(thd->is_error()))
    {
      error= 1;
      DBUG_RETURN(1);
    }

    /*
      If we are using ORDER BY NULL or ORDER BY const_expression,
      return result in any order (even if we are using a GROUP BY)
    */
    if (!order && org_order)
      skip_sort_order= 1;
  }
  /*
     Check if we can optimize away GROUP BY/DISTINCT.
     We can do that if there are no aggregate functions, the
     fields in DISTINCT clause (if present) and/or columns in GROUP BY
     (if present) contain direct references to all key parts of
     an unique index (in whatever order) and if the key parts of the
     unique index cannot contain NULLs.
     Note that the unique keys for DISTINCT and GROUP BY should not
     be the same (as long as they are unique).

     The FROM clause must contain a single non-constant table.
  */
  if (table_count - const_tables == 1 && (group || select_distinct) &&
      !tmp_table_param.sum_func_count &&
      (!join_tab[const_tables].select ||
       !join_tab[const_tables].select->quick ||
       join_tab[const_tables].select->quick->get_type() != 
       QUICK_SELECT_I::QS_TYPE_GROUP_MIN_MAX) &&
      !select_lex->have_window_funcs())
  {
    if (group && rollup.state == ROLLUP::STATE_NONE &&
       list_contains_unique_index(join_tab[const_tables].table,
                                 find_field_in_order_list,
                                 (void *) group_list))
    {
      /*
        We have found that grouping can be removed since groups correspond to
        only one row anyway, but we still have to guarantee correct result
        order. The line below effectively rewrites the query from GROUP BY
        <fields> to ORDER BY <fields>. There are three exceptions:
        - if skip_sort_order is set (see above), then we can simply skip
          GROUP BY;
        - if we are in a subquery, we don't have to maintain order unless there
	  is a limit clause in the subquery.
        - we can only rewrite ORDER BY if the ORDER BY fields are 'compatible'
          with the GROUP BY ones, i.e. either one is a prefix of another.
          We only check if the ORDER BY is a prefix of GROUP BY. In this case
          test_if_subpart() copies the ASC/DESC attributes from the original
          ORDER BY fields.
          If GROUP BY is a prefix of ORDER BY, then it is safe to leave
          'order' as is.
       */
      if (!order || test_if_subpart(group_list, order))
      {
        if (skip_sort_order ||
            (select_lex->master_unit()->item && select_limit == HA_POS_ERROR)) // This is a subquery
          order= NULL;
        else
          order= group_list;
      }
      /*
        If we have an IGNORE INDEX FOR GROUP BY(fields) clause, this must be 
        rewritten to IGNORE INDEX FOR ORDER BY(fields).
      */
      join_tab->table->keys_in_use_for_order_by=
        join_tab->table->keys_in_use_for_group_by;
      group_list= 0;
      group= 0;
    }
    if (select_distinct &&
       list_contains_unique_index(join_tab[const_tables].table,
                                 find_field_in_item_list,
                                 (void *) &fields_list))
    {
      select_distinct= 0;
    }
  }
  if (group || tmp_table_param.sum_func_count)
  {
    if (! hidden_group_fields && rollup.state == ROLLUP::STATE_NONE
        && !select_lex->have_window_funcs())
      select_distinct=0;
  }
  else if (select_distinct && table_count - const_tables == 1 &&
           rollup.state == ROLLUP::STATE_NONE &&
           !select_lex->have_window_funcs())
  {
    /*
      We are only using one table. In this case we change DISTINCT to a
      GROUP BY query if:
      - The GROUP BY can be done through indexes (no sort) and the ORDER
        BY only uses selected fields.
	(In this case we can later optimize away GROUP BY and ORDER BY)
      - We are scanning the whole table without LIMIT
        This can happen if:
        - We are using CALC_FOUND_ROWS
        - We are using an ORDER BY that can't be optimized away.

      We don't want to use this optimization when we are using LIMIT
      because in this case we can just create a temporary table that
      holds LIMIT rows and stop when this table is full.
    */
    bool all_order_fields_used;

    tab= &join_tab[const_tables];
    if (order)
    {
      skip_sort_order=
        test_if_skip_sort_order(tab, order, select_limit,
                                true,           // no_changes
                                &tab->table->keys_in_use_for_order_by);
    }
    if ((group_list=create_distinct_group(thd, select_lex->ref_pointer_array,
                                          order, fields_list, all_fields,
				          &all_order_fields_used)))
    {
      const bool skip_group=
        skip_sort_order &&
        test_if_skip_sort_order(tab, group_list, select_limit,
                                  true,         // no_changes
                                  &tab->table->keys_in_use_for_group_by);
      count_field_types(select_lex, &tmp_table_param, all_fields, 0);
      if ((skip_group && all_order_fields_used) ||
	  select_limit == HA_POS_ERROR ||
	  (order && !skip_sort_order))
      {
	/*  Change DISTINCT to GROUP BY */
	select_distinct= 0;
	no_order= !order;
	if (all_order_fields_used)
	{
	  if (order && skip_sort_order)
	  {
	    /*
	      Force MySQL to read the table in sorted order to get result in
	      ORDER BY order.
	    */
	    tmp_table_param.quick_group=0;
	  }
	  order=0;
        }
	group=1;				// For end_write_group
      }
      else
	group_list= 0;
    }
    else if (thd->is_fatal_error)			// End of memory
      DBUG_RETURN(1);
  }
  simple_group= rollup.state == ROLLUP::STATE_NONE;
  if (group)
  {
    /*
      Update simple_group and group_list as we now have more information, like
      which tables or columns are constant.
    */
    group_list= remove_const(this, group_list, conds,
                             rollup.state == ROLLUP::STATE_NONE,
                             &simple_group);
    if (unlikely(thd->is_error()))
    {
      error= 1;
      DBUG_RETURN(1);
    }
    if (!group_list)
    {
      /* The output has only one row */
      order=0;
      simple_order=1;
      select_distinct= 0;
      group_optimized_away= 1;
    }
  }

  calc_group_buffer(this, group_list);
  send_group_parts= tmp_table_param.group_parts; /* Save org parts */
  if (procedure && procedure->group)
  {
    group_list= procedure->group= remove_const(this, procedure->group, conds,
					       1, &simple_group);
    if (unlikely(thd->is_error()))
    {
      error= 1;
      DBUG_RETURN(1);
    }   
    calc_group_buffer(this, group_list);
  }

  if (test_if_subpart(group_list, order) ||
      (!group_list && tmp_table_param.sum_func_count))
  {
    order=0;
    if (is_indexed_agg_distinct(this, NULL))
      sort_and_group= 0;
  }

  // Can't use sort on head table if using join buffering
  if (full_join || hash_join)
  {
    TABLE *stable= (sort_by_table == (TABLE *) 1 ? 
      join_tab[const_tables].table : sort_by_table);
    /* 
      FORCE INDEX FOR ORDER BY can be used to prevent join buffering when
      sorting on the first table.
    */
    if (!stable || (!stable->force_index_order &&
                    !map2table[stable->tablenr]->keep_current_rowid))
    {
      if (group_list)
        simple_group= 0;
      if (order)
        simple_order= 0;
    }
  }

  need_tmp= test_if_need_tmp_table();

  /*
    If window functions are present then we can't have simple_order set to
    TRUE as the window function needs a temp table for computation.
    ORDER BY is computed after the window function computation is done, so
    the sort will be done on the temp table.
  */
  if (select_lex->have_window_funcs())
    simple_order= FALSE;


  /*
    If the hint FORCE INDEX FOR ORDER BY/GROUP BY is used for the table
    whose columns are required to be returned in a sorted order, then
    the proper value for no_jbuf_after should be yielded by a call to
    the make_join_orderinfo function.
    Yet the current implementation of FORCE INDEX hints does not
    allow us to do it in a clean manner.
  */
  no_jbuf_after= 1 ? table_count : make_join_orderinfo(this);

  // Don't use join buffering when we use MATCH
  select_opts_for_readinfo=
    (select_options & (SELECT_DESCRIBE | SELECT_NO_JOIN_CACHE)) |
    (select_lex->ftfunc_list->elements ?  SELECT_NO_JOIN_CACHE : 0);

  if (select_lex->options & OPTION_SCHEMA_TABLE &&
       optimize_schema_tables_reads(this))
    DBUG_RETURN(1);

  if (make_join_readinfo(this, select_opts_for_readinfo, no_jbuf_after))
    DBUG_RETURN(1);

  /* Perform FULLTEXT search before all regular searches */
  if (!(select_options & SELECT_DESCRIBE))
    if (init_ftfuncs(thd, select_lex, MY_TEST(order)))
      DBUG_RETURN(1);

  /*
    It's necessary to check const part of HAVING cond as
    there is a chance that some cond parts may become
    const items after make_join_statistics(for example
    when Item is a reference to cost table field from
    outer join).
    This check is performed only for those conditions
    which do not use aggregate functions. In such case
    temporary table may not be used and const condition
    elements may be lost during further having
    condition transformation in JOIN::exec.
  */
  if (having && const_table_map && !having->with_sum_func)
  {
    having->update_used_tables();
    having= having->remove_eq_conds(thd, &select_lex->having_value, true);
    if (select_lex->having_value == Item::COND_FALSE)
    {
      having= new (thd->mem_root) Item_int(thd, (longlong) 0,1);
      zero_result_cause= "Impossible HAVING noticed after reading const tables";
      error= 0;
      select_lex->mark_const_derived(zero_result_cause);
      goto setup_subq_exit;
    }
  }

  if (optimize_unflattened_subqueries())
    DBUG_RETURN(1);
  
  int res;
  if ((res= rewrite_to_index_subquery_engine(this)) != -1)
    DBUG_RETURN(res);
  if (setup_subquery_caches())
    DBUG_RETURN(-1);

  /*
    Need to tell handlers that to play it safe, it should fetch all
    columns of the primary key of the tables: this is because MySQL may
    build row pointers for the rows, and for all columns of the primary key
    the read set has not necessarily been set by the server code.
  */
  if (need_tmp || select_distinct || group_list || order)
  {
    for (uint i= 0; i < table_count; i++)
    {
      if (!(table[i]->map & const_table_map))
        table[i]->prepare_for_position();
    }
  }

  DBUG_EXECUTE("info",TEST_join(this););

  if (!only_const_tables())
  {
     JOIN_TAB *tab= &join_tab[const_tables];

    if (order)
    {
      /*
        Force using of tmp table if sorting by a SP or UDF function due to
        their expensive and probably non-deterministic nature.
      */
      for (ORDER *tmp_order= order; tmp_order ; tmp_order=tmp_order->next)
      {
        Item *item= *tmp_order->item;
        if (item->is_expensive())
        {
          /* Force tmp table without sort */
          need_tmp=1; simple_order=simple_group=0;
          break;
        }
      }
    }

    /*
      Because filesort always does a full table scan or a quick range scan
      we must add the removed reference to the select for the table.
      We only need to do this when we have a simple_order or simple_group
      as in other cases the join is done before the sort.
    */
    if ((order || group_list) &&
        tab->type != JT_ALL &&
        tab->type != JT_FT &&
        tab->type != JT_REF_OR_NULL &&
        ((order && simple_order) || (group_list && simple_group)))
    {
      if (add_ref_to_table_cond(thd,tab)) {
        DBUG_RETURN(1);
      }
    }
    /*
      Investigate whether we may use an ordered index as part of either
      DISTINCT, GROUP BY or ORDER BY execution. An ordered index may be
      used for only the first of any of these terms to be executed. This
      is reflected in the order which we check for test_if_skip_sort_order()
      below. However we do not check for DISTINCT here, as it would have
      been transformed to a GROUP BY at this stage if it is a candidate for 
      ordered index optimization.
      If a decision was made to use an ordered index, the availability
      of such an access path is stored in 'ordered_index_usage' for later
      use by 'execute' or 'explain'
    */
    DBUG_ASSERT(ordered_index_usage == ordered_index_void);

    if (group_list)   // GROUP BY honoured first
                      // (DISTINCT was rewritten to GROUP BY if skippable)
    {
      /*
        When there is SQL_BIG_RESULT do not sort using index for GROUP BY,
        and thus force sorting on disk unless a group min-max optimization
        is going to be used as it is applied now only for one table queries
        with covering indexes.
      */
      if (!(select_options & SELECT_BIG_RESULT) ||
            (tab->select &&
             tab->select->quick &&
             tab->select->quick->get_type() ==
             QUICK_SELECT_I::QS_TYPE_GROUP_MIN_MAX))
      {
        if (simple_group &&              // GROUP BY is possibly skippable
            !select_distinct)            // .. if not preceded by a DISTINCT
        {
          /*
            Calculate a possible 'limit' of table rows for 'GROUP BY':
            A specified 'LIMIT' is relative to the final resultset.
            'need_tmp' implies that there will be more postprocessing 
            so the specified 'limit' should not be enforced yet.
           */
          const ha_rows limit = need_tmp ? HA_POS_ERROR : select_limit;
          if (test_if_skip_sort_order(tab, group_list, limit, false, 
                                      &tab->table->keys_in_use_for_group_by))
          {
            ordered_index_usage= ordered_index_group_by;
          }
        }

	/*
	  If we are going to use semi-join LooseScan, it will depend
	  on the selected index scan to be used.  If index is not used
	  for the GROUP BY, we risk that sorting is put on the LooseScan
	  table.  In order to avoid this, force use of temporary table.
	  TODO: Explain the quick_group part of the test below.
	 */
        if ((ordered_index_usage != ordered_index_group_by) &&
            ((tmp_table_param.quick_group && !procedure) || 
	     (tab->emb_sj_nest && 
	      best_positions[const_tables].sj_strategy == SJ_OPT_LOOSE_SCAN)))
        {
          need_tmp=1;
          simple_order= simple_group= false; // Force tmp table without sort
        }
      }
    }
    else if (order &&                      // ORDER BY wo/ preceding GROUP BY
             (simple_order || skip_sort_order)) // which is possibly skippable
    {
      if (test_if_skip_sort_order(tab, order, select_limit, false, 
                                  &tab->table->keys_in_use_for_order_by))
      {
        ordered_index_usage= ordered_index_order_by;
      }
    }
  }

  if (having)
    having_is_correlated= MY_TEST(having->used_tables() & OUTER_REF_TABLE_BIT);
  tmp_having= having;

  if (unlikely(thd->is_error()))
    DBUG_RETURN(TRUE);

  /*
    The loose index scan access method guarantees that all grouping or
    duplicate row elimination (for distinct) is already performed
    during data retrieval, and that all MIN/MAX functions are already
    computed for each group. Thus all MIN/MAX functions should be
    treated as regular functions, and there is no need to perform
    grouping in the main execution loop.
    Notice that currently loose index scan is applicable only for
    single table queries, thus it is sufficient to test only the first
    join_tab element of the plan for its access method.
  */
  if (join_tab->is_using_loose_index_scan())
  {
    tmp_table_param.precomputed_group_by= TRUE;
    if (join_tab->is_using_agg_loose_index_scan())
    {
      need_distinct= FALSE;
      tmp_table_param.precomputed_group_by= FALSE;
    }
  }

  if (make_aggr_tables_info())
    DBUG_RETURN(1);

  if (init_join_caches())
    DBUG_RETURN(1);

  error= 0;

  if (select_options & SELECT_DESCRIBE)
    goto derived_exit;

  DBUG_RETURN(0);

setup_subq_exit:
  /* Choose an execution strategy for this JOIN. */
  if (!tables_list || !table_count)
  {
    choose_tableless_subquery_plan();

    /* The output has atmost one row */
    if (group_list)
    {
      group_list= NULL;
      group_optimized_away= 1;
      rollup.state= ROLLUP::STATE_NONE;
    }
    order= NULL;
    simple_order= TRUE;
    select_distinct= FALSE;

    if (select_lex->have_window_funcs())
    {
      if (!(join_tab= (JOIN_TAB*) thd->alloc(sizeof(JOIN_TAB))))
        DBUG_RETURN(1);
      need_tmp= 1;
    }
    if (make_aggr_tables_info())
      DBUG_RETURN(1);
  }
  /*
    Even with zero matching rows, subqueries in the HAVING clause may
    need to be evaluated if there are aggregate functions in the query.
  */
  if (optimize_unflattened_subqueries())
    DBUG_RETURN(1);
  error= 0;

derived_exit:

  select_lex->mark_const_derived(zero_result_cause);
  DBUG_RETURN(0);
}