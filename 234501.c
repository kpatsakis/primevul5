remove_const(JOIN *join,ORDER *first_order, COND *cond,
             bool change_list, bool *simple_order)
{
  *simple_order= join->rollup.state == ROLLUP::STATE_NONE;
  if (join->only_const_tables())
    return change_list ? 0 : first_order;		// No need to sort

  ORDER *order,**prev_ptr, *tmp_order;
  table_map UNINIT_VAR(first_table); /* protected by first_is_base_table */
  table_map not_const_tables= ~join->const_table_map;
  table_map ref;
  bool first_is_base_table= FALSE;
  DBUG_ENTER("remove_const");
  
  /*
    Join tab is set after make_join_statistics() has been called.
    In case of one table with GROUP BY this function is called before
    join_tab is set for the GROUP_BY expression
  */
  if (join->join_tab)
  {
    if (join->join_tab[join->const_tables].table)
    {
      first_table= join->join_tab[join->const_tables].table->map;
      first_is_base_table= TRUE;
    }
  
    /*
      Cleanup to avoid interference of calls of this function for
      ORDER BY and GROUP BY
    */
    for (JOIN_TAB *tab= join->join_tab + join->const_tables;
         tab < join->join_tab + join->table_count;
         tab++)
      tab->cached_eq_ref_table= FALSE;

    *simple_order= *join->join_tab[join->const_tables].on_expr_ref ? 0 : 1;
  }
  else
  {
    first_is_base_table= FALSE;
    first_table= 0;                     // Not used, for gcc
  }

  prev_ptr= &first_order;

  /* NOTE: A variable of not_const_tables ^ first_table; breaks gcc 2.7 */

  update_depend_map_for_order(join, first_order);
  for (order=first_order; order ; order=order->next)
  {
    table_map order_tables=order->item[0]->used_tables();
    if (order->item[0]->with_sum_func ||
        order->item[0]->with_window_func ||
        /*
          If the outer table of an outer join is const (either by itself or
          after applying WHERE condition), grouping on a field from such a
          table will be optimized away and filesort without temporary table
          will be used unless we prevent that now. Filesort is not fit to
          handle joins and the join condition is not applied. We can't detect
          the case without an expensive test, however, so we force temporary
          table for all queries containing more than one table, ROLLUP, and an
          outer join.
         */
        (join->table_count > 1 && join->rollup.state == ROLLUP::STATE_INITED &&
        join->outer_join))
      *simple_order=0;				// Must do a temp table to sort
    else if (!(order_tables & not_const_tables))
    {
      if (order->item[0]->with_subquery())
      {
        /*
          Delay the evaluation of constant ORDER and/or GROUP expressions that
          contain subqueries until the execution phase.
        */
        join->exec_const_order_group_cond.push_back(order->item[0],
                                                    join->thd->mem_root);
      }
      DBUG_PRINT("info",("removing: %s", order->item[0]->full_name()));
      continue;
    }
    else
    {
      if (order_tables & (RAND_TABLE_BIT | OUTER_REF_TABLE_BIT))
	*simple_order=0;
      else
      {
	if (cond && const_expression_in_where(cond,order->item[0]))
	{
	  DBUG_PRINT("info",("removing: %s", order->item[0]->full_name()));
	  continue;
	}
	if (first_is_base_table &&
            (ref=order_tables & (not_const_tables ^ first_table)))
	{
	  if (!(order_tables & first_table) &&
              only_eq_ref_tables(join,first_order, ref))
	  {
	    DBUG_PRINT("info",("removing: %s", order->item[0]->full_name()));
	    continue;
	  }
          /*
            UseMultipleEqualitiesToRemoveTempTable:
            Can use multiple-equalities here to check that ORDER BY columns
            can be used without tmp. table.
          */
          bool can_subst_to_first_table= false;
          bool first_is_in_sjm_nest= false;
          if (first_is_base_table)
          {
            TABLE_LIST *tbl_for_first=
              join->join_tab[join->const_tables].table->pos_in_table_list;
            first_is_in_sjm_nest= tbl_for_first->sj_mat_info &&
                                  tbl_for_first->sj_mat_info->is_used;
          }
          /*
            Currently we do not employ the optimization that uses multiple
            equalities for ORDER BY to remove tmp table in the case when
            the first table happens to be the result of materialization of
            a semi-join nest ( <=> first_is_in_sjm_nest == true).

            When a semi-join nest is materialized and scanned to look for
            possible matches in the remaining tables for every its row
            the fields from the result of materialization are copied
            into the record buffers of tables from the semi-join nest.
            So these copies are used to access the remaining tables rather
            than the fields from the result of materialization.

            Unfortunately now this so-called 'copy back' technique is
            supported only if the rows  are scanned with the rr_sequential
            function, but not with other rr_* functions that are employed
            when the result of materialization is required to be sorted.

            TODO: either to support 'copy back' technique for the above case,
                  or to get rid of this technique altogether.
          */
          if (optimizer_flag(join->thd, OPTIMIZER_SWITCH_ORDERBY_EQ_PROP) &&
              first_is_base_table && !first_is_in_sjm_nest &&
              order->item[0]->real_item()->type() == Item::FIELD_ITEM &&
              join->cond_equal)
          {
            table_map first_table_bit=
              join->join_tab[join->const_tables].table->map;

            Item *item= order->item[0];

            /*
              TODO: equality substitution in the context of ORDER BY is 
              sometimes allowed when it is not allowed in the general case.
              
              We make the below call for its side effect: it will locate the
              multiple equality the item belongs to and set item->item_equal
              accordingly.
            */
            Item *res= item->propagate_equal_fields(join->thd,
                                                    Value_source::
                                                    Context_identity(),
                                                    join->cond_equal);
            Item_equal *item_eq;
            if ((item_eq= res->get_item_equal()))
            {
              Item *first= item_eq->get_first(NO_PARTICULAR_TAB, NULL);
              if (first->const_item() || first->used_tables() ==
                                         first_table_bit)
              {
                can_subst_to_first_table= true;
              }
            }
          }

          if (!can_subst_to_first_table)
          {
            *simple_order=0;			// Must do a temp table to sort
          }
	}
      }
    }
    /* Remove ORDER BY entries that we have seen before */
    for (tmp_order= first_order;
         tmp_order != order;
         tmp_order= tmp_order->next)
    {
      if (tmp_order->item[0]->eq(order->item[0],1))
        break;
    }
    if (tmp_order != order)
      continue;                                // Duplicate order by. Remove
    
    if (change_list)
      *prev_ptr= order;				// use this entry
    prev_ptr= &order->next;
  }
  if (change_list)
    *prev_ptr=0;
  if (prev_ptr == &first_order)			// Nothing to sort/group
    *simple_order=1;
#ifndef DBUG_OFF
  if (unlikely(join->thd->is_error()))
    DBUG_PRINT("error",("Error from remove_const"));
#endif
  DBUG_PRINT("exit",("simple_order: %d",(int) *simple_order));
  DBUG_RETURN(first_order);
}