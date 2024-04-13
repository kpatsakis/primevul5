void JOIN::exec_inner()
{
  List<Item> *columns_list= &fields_list;
  DBUG_ENTER("JOIN::exec_inner");
  DBUG_ASSERT(optimization_state == JOIN::OPTIMIZATION_DONE);

  THD_STAGE_INFO(thd, stage_executing);

  /*
    Enable LIMIT ROWS EXAMINED during query execution if:
    (1) This JOIN is the outermost query (not a subquery or derived table)
        This ensures that the limit is enabled when actual execution begins, and
        not if a subquery is evaluated during optimization of the outer query.
    (2) This JOIN is not the result of a UNION. In this case do not apply the
        limit in order to produce the partial query result stored in the
        UNION temp table.
  */
  if (!select_lex->outer_select() &&                            // (1)
      select_lex != select_lex->master_unit()->fake_select_lex) // (2)
    thd->lex->set_limit_rows_examined();

  if (procedure)
  {
    procedure_fields_list= fields_list;
    if (procedure->change_columns(thd, procedure_fields_list) ||
	result->prepare(procedure_fields_list, unit))
    {
      thd->set_examined_row_count(0);
      thd->limit_found_rows= 0;
      DBUG_VOID_RETURN;
    }
    columns_list= &procedure_fields_list;
  }
  if (result->prepare2())
    DBUG_VOID_RETURN;

  if (!tables_list && (table_count || !select_lex->with_sum_func) &&
      !select_lex->have_window_funcs())
  {                                           // Only test of functions
    if (select_options & SELECT_DESCRIBE)
      select_describe(this, FALSE, FALSE, FALSE,
		      (zero_result_cause?zero_result_cause:"No tables used"));

    else
    {
      if (result->send_result_set_metadata(*columns_list,
                                           Protocol::SEND_NUM_ROWS |
                                           Protocol::SEND_EOF))
      {
        DBUG_VOID_RETURN;
      }

      /*
        We have to test for 'conds' here as the WHERE may not be constant
        even if we don't have any tables for prepared statements or if
        conds uses something like 'rand()'.
        If the HAVING clause is either impossible or always true, then
        JOIN::having is set to NULL by optimize_cond.
        In this case JOIN::exec must check for JOIN::having_value, in the
        same way it checks for JOIN::cond_value.
      */
      DBUG_ASSERT(error == 0);
      if (cond_value != Item::COND_FALSE &&
          having_value != Item::COND_FALSE &&
          (!conds || conds->val_int()) &&
          (!having || having->val_int()))
      {
	if (do_send_rows &&
            (procedure ? (procedure->send_row(procedure_fields_list) ||
             procedure->end_of_records()) : result->send_data(fields_list)> 0))
	  error= 1;
	else
	  send_records= ((select_options & OPTION_FOUND_ROWS) ? 1 :
                         thd->get_sent_row_count());
      }
      else
        send_records= 0;
      if (!error)
      {
        join_free();                      // Unlock all cursors
        error= (int) result->send_eof();
      }
    }
    /* Single select (without union) always returns 0 or 1 row */
    thd->limit_found_rows= send_records;
    thd->set_examined_row_count(0);
    DBUG_VOID_RETURN;
  }

  /*
    Evaluate expensive constant conditions that were not evaluated during
    optimization. Do not evaluate them for EXPLAIN statements as these
    condtions may be arbitrarily costly, and because the optimize phase
    might not have produced a complete executable plan for EXPLAINs.
  */
  if (!zero_result_cause &&
      exec_const_cond && !(select_options & SELECT_DESCRIBE) &&
      !exec_const_cond->val_int())
    zero_result_cause= "Impossible WHERE noticed after reading const tables";

  /* 
    We've called exec_const_cond->val_int(). This may have caused an error.
  */
  if (thd->is_error())
  {
    error= thd->is_error();
    DBUG_VOID_RETURN;
  }

  if (zero_result_cause)
  {
    if (select_lex->have_window_funcs() && send_row_on_empty_set())
    {
      /*
        The query produces just one row but it has window functions.

        The only way to compute the value of window function(s) is to
        run the entire window function computation step (there is no shortcut).
      */
      const_tables= table_count;
      first_select= sub_select_postjoin_aggr;
    }
    else
    {
      (void) return_zero_rows(this, result, select_lex->leaf_tables,
                              *columns_list,
			      send_row_on_empty_set(),
			      select_options,
			      zero_result_cause,
			      having ? having : tmp_having, all_fields);
      DBUG_VOID_RETURN;
    }
  }
  
  /*
    Evaluate all constant expressions with subqueries in the
    ORDER/GROUP clauses to make sure that all subqueries return a
    single row. The evaluation itself will trigger an error if that is
    not the case.
  */
  if (exec_const_order_group_cond.elements &&
      !(select_options & SELECT_DESCRIBE))
  {
    List_iterator_fast<Item> const_item_it(exec_const_order_group_cond);
    Item *cur_const_item;
    while ((cur_const_item= const_item_it++))
    {
      cur_const_item->val_str(); // This caches val_str() to Item::str_value
      if (thd->is_error())
      {
        error= thd->is_error();
        DBUG_VOID_RETURN;
      }
    }
  }

  if ((this->select_lex->options & OPTION_SCHEMA_TABLE) &&
      get_schema_tables_result(this, PROCESSED_BY_JOIN_EXEC))
    DBUG_VOID_RETURN;

  if (select_options & SELECT_DESCRIBE)
  {
    select_describe(this, need_tmp,
		    order != 0 && !skip_sort_order,
		    select_distinct,
                    !table_count ? "No tables used" : NullS);
    DBUG_VOID_RETURN;
  }
  else
  {
    /* it's a const select, materialize it. */
    select_lex->mark_const_derived(zero_result_cause);
  }

  /*
    Initialize examined rows here because the values from all join parts
    must be accumulated in examined_row_count. Hence every join
    iteration must count from zero.
  */
  join_examined_rows= 0;

  /* XXX: When can we have here thd->is_error() not zero? */
  if (thd->is_error())
  {
    error= thd->is_error();
    DBUG_VOID_RETURN;
  }

  THD_STAGE_INFO(thd, stage_sending_data);
  DBUG_PRINT("info", ("%s", thd->proc_info));
  result->send_result_set_metadata(
                 procedure ? procedure_fields_list : *fields,
                 Protocol::SEND_NUM_ROWS | Protocol::SEND_EOF);
  error= do_select(this, procedure);
  /* Accumulate the counts from all join iterations of all join parts. */
  thd->inc_examined_row_count(join_examined_rows);
  DBUG_PRINT("counts", ("thd->examined_row_count: %lu",
                        (ulong) thd->get_examined_row_count()));

  DBUG_VOID_RETURN;
}