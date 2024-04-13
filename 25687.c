JOIN::prepare(TABLE_LIST *tables_init,
	      uint wild_num, COND *conds_init, uint og_num,
	      ORDER *order_init, bool skip_order_by,
              ORDER *group_init, Item *having_init,
	      ORDER *proc_param_init, SELECT_LEX *select_lex_arg,
	      SELECT_LEX_UNIT *unit_arg)
{
  DBUG_ENTER("JOIN::prepare");

  // to prevent double initialization on EXPLAIN
  if (optimization_state != JOIN::NOT_OPTIMIZED)
    DBUG_RETURN(0);

  conds= conds_init;
  order= order_init;
  group_list= group_init;
  having= having_init;
  proc_param= proc_param_init;
  tables_list= tables_init;
  select_lex= select_lex_arg;
  select_lex->join= this;
  join_list= &select_lex->top_join_list;
  union_part= unit_arg->is_union();

  // simple check that we got usable conds
  dbug_print_item(conds);

  if (select_lex->handle_derived(thd->lex, DT_PREPARE))
    DBUG_RETURN(1);

  thd->lex->current_select->context_analysis_place= NO_MATTER;
  thd->lex->current_select->is_item_list_lookup= 1;
  /*
    If we have already executed SELECT, then it have not sense to prevent
    its table from update (see unique_table())
    Affects only materialized derived tables.
  */
  /* Check that all tables, fields, conds and order are ok */
  if (!(select_options & OPTION_SETUP_TABLES_DONE) &&
      setup_tables_and_check_access(thd, &select_lex->context, join_list,
                                    tables_list, select_lex->leaf_tables,
                                    FALSE, SELECT_ACL, SELECT_ACL, FALSE))
      DBUG_RETURN(-1);
  
  /*
    TRUE if the SELECT list mixes elements with and without grouping,
    and there is no GROUP BY clause. Mixing non-aggregated fields with
    aggregate functions in the SELECT list is a MySQL exptenstion that
    is allowed only if the ONLY_FULL_GROUP_BY sql mode is not set.
  */
  mixed_implicit_grouping= false;
  if ((~thd->variables.sql_mode & MODE_ONLY_FULL_GROUP_BY) &&
      select_lex->with_sum_func && !group_list)
  {
    List_iterator_fast <Item> select_it(fields_list);
    Item *select_el; /* Element of the SELECT clause, can be an expression. */
    bool found_field_elem= false;
    bool found_sum_func_elem= false;

    while ((select_el= select_it++))
    {
      if (select_el->with_sum_func)
        found_sum_func_elem= true;
      if (select_el->with_field)
        found_field_elem= true;
      if (found_sum_func_elem && found_field_elem)
      {
        mixed_implicit_grouping= true;
        break;
      }
    }
  }

  table_count= select_lex->leaf_tables.elements;

  TABLE_LIST *tbl;
  List_iterator_fast<TABLE_LIST> li(select_lex->leaf_tables);
  while ((tbl= li++))
  {
    /*
      If the query uses implicit grouping where the select list contains both
      aggregate functions and non-aggregate fields, any non-aggregated field
      may produce a NULL value. Set all fields of each table as nullable before
      semantic analysis to take into account this change of nullability.

      Note: this loop doesn't touch tables inside merged semi-joins, because
      subquery-to-semijoin conversion has not been done yet. This is intended.
    */
    if (mixed_implicit_grouping && tbl->table)
      tbl->table->maybe_null= 1;
  }
 
  uint real_og_num= og_num;
  if (skip_order_by && 
      select_lex != select_lex->master_unit()->global_parameters())
    real_og_num+= select_lex->order_list.elements;

  DBUG_ASSERT(select_lex->hidden_bit_fields == 0);
  if (setup_wild(thd, tables_list, fields_list, &all_fields, wild_num,
                 &select_lex->hidden_bit_fields))
    DBUG_RETURN(-1);
  if (select_lex->setup_ref_array(thd, real_og_num))
    DBUG_RETURN(-1);

  ref_ptrs= ref_ptr_array_slice(0);
  
  enum_parsing_place save_place=
                     thd->lex->current_select->context_analysis_place;
  thd->lex->current_select->context_analysis_place= SELECT_LIST;
  if (setup_fields(thd, ref_ptrs, fields_list, MARK_COLUMNS_READ,
                   &all_fields, &select_lex->pre_fix, 1))
    DBUG_RETURN(-1);
  thd->lex->current_select->context_analysis_place= save_place;

  if (setup_without_group(thd, ref_ptrs, tables_list,
                          select_lex->leaf_tables, fields_list,
                          all_fields, &conds, order, group_list,
                          select_lex->window_specs,
                          select_lex->window_funcs,
                          &hidden_group_fields,
                          &select_lex->select_n_reserved))
    DBUG_RETURN(-1);

  /*
    Permanently remove redundant parts from the query if
      1) This is a subquery
      2) This is the first time this query is optimized (since the
         transformation is permanent
      3) Not normalizing a view. Removal should take place when a
         query involving a view is optimized, not when the view
         is created
  */
  if (select_lex->master_unit()->item &&                               // 1)
      select_lex->first_cond_optimization &&                           // 2)
      !thd->lex->is_view_context_analysis())                           // 3)
  {
    remove_redundant_subquery_clauses(select_lex);
  }

  /* Resolve the ORDER BY that was skipped, then remove it. */
  if (skip_order_by && select_lex !=
                       select_lex->master_unit()->global_parameters())
  {
    nesting_map save_allow_sum_func= thd->lex->allow_sum_func;
    thd->lex->allow_sum_func|= (nesting_map)1 << select_lex->nest_level;
    thd->where= "order clause";
    for (ORDER *order= select_lex->order_list.first; order; order= order->next)
    {
      /* Don't add the order items to all fields. Just resolve them to ensure
         the query is valid, we'll drop them immediately after. */
      if (find_order_in_list(thd, ref_ptrs, tables_list, order,
                             fields_list, all_fields, false, false, false))
        DBUG_RETURN(-1);
    }
    thd->lex->allow_sum_func= save_allow_sum_func;
    select_lex->order_list.empty();
  }

  if (having)
  {
    nesting_map save_allow_sum_func= thd->lex->allow_sum_func;
    thd->where="having clause";
    thd->lex->allow_sum_func|= (nesting_map)1 << select_lex_arg->nest_level;
    select_lex->having_fix_field= 1;
    /*
      Wrap alone field in HAVING clause in case it will be outer field
      of subquery which need persistent pointer on it, but having
      could be changed by optimizer
    */
    if (having->type() == Item::REF_ITEM &&
        ((Item_ref *)having)->ref_type() == Item_ref::REF)
      wrap_ident(thd, &having);
    bool having_fix_rc= (!having->fixed &&
			 (having->fix_fields(thd, &having) ||
			  having->check_cols(1)));
    select_lex->having_fix_field= 0;

    if (having_fix_rc || thd->is_error())
      DBUG_RETURN(-1);				/* purecov: inspected */
    thd->lex->allow_sum_func= save_allow_sum_func;

    if (having->with_window_func)
    {
      my_error(ER_WRONG_PLACEMENT_OF_WINDOW_FUNCTION, MYF(0));
      DBUG_RETURN(-1); 
    }
  }

  /*
     After setting up window functions, we may have discovered additional
     used tables from the PARTITION BY and ORDER BY list. Update all items
     that contain window functions.
  */
  if (select_lex->have_window_funcs())
  {
    List_iterator_fast<Item> it(select_lex->item_list);
    Item *item;
    while ((item= it++))
    {
      if (item->with_window_func)
        item->update_used_tables();
    }
  }

  With_clause *with_clause=select_lex->get_with_clause();
  if (with_clause && with_clause->prepare_unreferenced_elements(thd))
    DBUG_RETURN(1);

  With_element *with_elem= select_lex->get_with_element();
  if (with_elem &&
      select_lex->check_unrestricted_recursive(
                      thd->variables.only_standard_compliant_cte))
    DBUG_RETURN(-1);
  if (!(select_lex->changed_elements & TOUCHED_SEL_COND))
    select_lex->check_subqueries_with_recursive_references();
  
  int res= check_and_do_in_subquery_rewrites(this);

  select_lex->fix_prepare_information(thd, &conds, &having);
  
  if (res)
    DBUG_RETURN(res);

  if (order)
  {
    bool real_order= FALSE;
    ORDER *ord;
    for (ord= order; ord; ord= ord->next)
    {
      Item *item= *ord->item;
      /*
        Disregard sort order if there's only 
        zero length NOT NULL fields (e.g. {VAR}CHAR(0) NOT NULL") or
        zero length NOT NULL string functions there.
        Such tuples don't contain any data to sort.
      */
      if (!real_order &&
           /* Not a zero length NOT NULL field */
          ((item->type() != Item::FIELD_ITEM ||
            ((Item_field *) item)->field->maybe_null() ||
            ((Item_field *) item)->field->sort_length()) &&
           /* AND not a zero length NOT NULL string function. */
           (item->type() != Item::FUNC_ITEM ||
            item->maybe_null ||
            item->result_type() != STRING_RESULT ||
            item->max_length)))
        real_order= TRUE;

      if ((item->with_sum_func && item->type() != Item::SUM_FUNC_ITEM) ||
          item->with_window_func)
        item->split_sum_func(thd, ref_ptrs, all_fields, SPLIT_SUM_SELECT);
    }
    if (!real_order)
      order= NULL;
  }

  if (having && having->with_sum_func)
    having->split_sum_func2(thd, ref_ptrs, all_fields,
                            &having, SPLIT_SUM_SKIP_REGISTERED);
  if (select_lex->inner_sum_func_list)
  {
    Item_sum *end=select_lex->inner_sum_func_list;
    Item_sum *item_sum= end;  
    do
    { 
      item_sum= item_sum->next;
      item_sum->split_sum_func2(thd, ref_ptrs,
                                all_fields, item_sum->ref_by, 0);
    } while (item_sum != end);
  }

  if (select_lex->inner_refs_list.elements &&
      fix_inner_refs(thd, all_fields, select_lex, ref_ptrs))
    DBUG_RETURN(-1);

  if (group_list)
  {
    /*
      Because HEAP tables can't index BIT fields we need to use an
      additional hidden field for grouping because later it will be
      converted to a LONG field. Original field will remain of the
      BIT type and will be returned to a client.
    */
    for (ORDER *ord= group_list; ord; ord= ord->next)
    {
      if ((*ord->item)->type() == Item::FIELD_ITEM &&
          (*ord->item)->field_type() == MYSQL_TYPE_BIT)
      {
        Item_field *field= new (thd->mem_root) Item_field(thd, *(Item_field**)ord->item);
        int el= all_fields.elements;
        ref_ptrs[el]= field;
        all_fields.push_front(field, thd->mem_root);
        ord->item= &ref_ptrs[el];
      }
    }
  }

  /*
    Check if there are references to un-aggregated columns when computing 
    aggregate functions with implicit grouping (there is no GROUP BY).
  */
  if (thd->variables.sql_mode & MODE_ONLY_FULL_GROUP_BY && !group_list &&
      !(select_lex->master_unit()->item &&
        select_lex->master_unit()->item->is_in_predicate() &&
        ((Item_in_subselect*)select_lex->master_unit()->item)->
        test_set_strategy(SUBS_MAXMIN_INJECTED)) &&
      select_lex->non_agg_field_used() &&
      select_lex->agg_func_used())
  {
    my_message(ER_MIX_OF_GROUP_FUNC_AND_FIELDS,
               ER_THD(thd, ER_MIX_OF_GROUP_FUNC_AND_FIELDS), MYF(0));
    DBUG_RETURN(-1);
  }
  {
    /* Caclulate the number of groups */
    send_group_parts= 0;
    for (ORDER *group_tmp= group_list ; group_tmp ; group_tmp= group_tmp->next)
      send_group_parts++;
  }
  
  procedure= setup_procedure(thd, proc_param, result, fields_list, &error);
  if (error)
    goto err;					/* purecov: inspected */
  if (procedure)
  {
    if (setup_new_fields(thd, fields_list, all_fields,
			 procedure->param_fields))
	goto err;				/* purecov: inspected */
    if (procedure->group)
    {
      if (!test_if_subpart(procedure->group,group_list))
      {						/* purecov: inspected */
	my_message(ER_DIFF_GROUPS_PROC, ER_THD(thd, ER_DIFF_GROUPS_PROC),
                   MYF(0));                     /* purecov: inspected */
	goto err;				/* purecov: inspected */
      }
    }
    if (order && (procedure->flags & PROC_NO_SORT))
    {						/* purecov: inspected */
      my_message(ER_ORDER_WITH_PROC, ER_THD(thd, ER_ORDER_WITH_PROC),
                 MYF(0));                       /* purecov: inspected */
      goto err;					/* purecov: inspected */
    }
    if (thd->lex->derived_tables)
    {
      /*
        Queries with derived tables and PROCEDURE are not allowed.
        Many of such queries are disallowed grammatically, but there
        are still some complex cases:
          SELECT 1 FROM (SELECT 1) a PROCEDURE ANALYSE()
      */
      my_error(ER_WRONG_USAGE, MYF(0), "PROCEDURE", 
               thd->lex->derived_tables & DERIVED_VIEW ?
               "view" : "subquery"); 
      goto err;
    }
    if (thd->lex->sql_command != SQLCOM_SELECT)
    {
      // EXPLAIN SELECT * FROM t1 PROCEDURE ANALYSE()
      my_error(ER_WRONG_USAGE, MYF(0), "PROCEDURE", "non-SELECT");
      goto err;
    }
  }

  if (!procedure && result && result->prepare(fields_list, unit_arg))
    goto err;					/* purecov: inspected */

  unit= unit_arg;
  if (prepare_stage2())
    goto err;

  DBUG_RETURN(0); // All OK

err:
  delete procedure;                /* purecov: inspected */
  procedure= 0;
  DBUG_RETURN(-1);                /* purecov: inspected */
}