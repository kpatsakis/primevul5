do_select(JOIN *join, Procedure *procedure)
{
  int rc= 0;
  enum_nested_loop_state error= NESTED_LOOP_OK;
  DBUG_ENTER("do_select");

  if (join->pushdown_query)
  {
    /* Select fields are in the temporary table */
    join->fields= &join->tmp_fields_list1;
    /* Setup HAVING to work with fields in temporary table */
    join->set_items_ref_array(join->items1);
    /* The storage engine will take care of the group by query result */
    int res= join->pushdown_query->execute(join);

    if (res)
      DBUG_RETURN(res);

    if (join->pushdown_query->store_data_in_temp_table)
    {
      JOIN_TAB *last_tab= join->join_tab + join->table_count -
                          join->exec_join_tab_cnt();      
      last_tab->next_select= end_send;

      enum_nested_loop_state state= last_tab->aggr->end_send();
      if (state >= NESTED_LOOP_OK)
        state= sub_select(join, last_tab, true);

      if (state < NESTED_LOOP_OK)
        res= 1;

      if (join->result->send_eof())
        res= 1;
    }
    DBUG_RETURN(res);
  }
  
  join->procedure= procedure;
  join->duplicate_rows= join->send_records=0;
  if (join->only_const_tables() && !join->need_tmp)
  {
    Next_select_func end_select= setup_end_select_func(join, NULL);
    /*
      HAVING will be checked after processing aggregate functions,
      But WHERE should checked here (we alredy have read tables).
      Notice that make_join_select() splits all conditions in this case
      into two groups exec_const_cond and outer_ref_cond.
      If join->table_count == join->const_tables then it is
      sufficient to check only the condition pseudo_bits_cond.
    */
    DBUG_ASSERT(join->outer_ref_cond == NULL);
    if (!join->pseudo_bits_cond || join->pseudo_bits_cond->val_int())
    {
      // HAVING will be checked by end_select
      error= (*end_select)(join, 0, 0);
      if (error >= NESTED_LOOP_OK)
	error= (*end_select)(join, 0, 1);

      /*
        If we don't go through evaluate_join_record(), do the counting
        here.  join->send_records is increased on success in end_send(),
        so we don't touch it here.
      */
      join->join_examined_rows++;
      DBUG_ASSERT(join->join_examined_rows <= 1);
    }
    else if (join->send_row_on_empty_set())
    {
      if (!join->having || join->having->val_int())
      {
        List<Item> *columns_list= (procedure ? &join->procedure_fields_list :
                                   join->fields);
        rc= join->result->send_data(*columns_list) > 0;
      }
    }
    /*
      An error can happen when evaluating the conds 
      (the join condition and piece of where clause 
      relevant to this join table).
    */
    if (join->thd->is_error())
      error= NESTED_LOOP_ERROR;
  }
  else
  {
    DBUG_EXECUTE_IF("show_explain_probe_do_select", 
                    if (dbug_user_var_equals_int(join->thd, 
                                                 "show_explain_probe_select_id", 
                                                 join->select_lex->select_number))
                          dbug_serve_apcs(join->thd, 1);
                   );

    JOIN_TAB *join_tab= join->join_tab +
                        (join->tables_list ? join->const_tables : 0);
    if (join->outer_ref_cond && !join->outer_ref_cond->val_int())
      error= NESTED_LOOP_NO_MORE_ROWS;
    else
      error= join->first_select(join,join_tab,0);
    if (error >= NESTED_LOOP_OK && join->thd->killed != ABORT_QUERY)
      error= join->first_select(join,join_tab,1);
  }

  join->thd->limit_found_rows= join->send_records - join->duplicate_rows;

  if (error == NESTED_LOOP_NO_MORE_ROWS || join->thd->killed == ABORT_QUERY)
    error= NESTED_LOOP_OK;

  /*
    For "order by with limit", we cannot rely on send_records, but need
    to use the rowcount read originally into the join_tab applying the
    filesort. There cannot be any post-filtering conditions, nor any
    following join_tabs in this case, so this rowcount properly represents
    the correct number of qualifying rows.
  */
  if (join->order)
  {
    // Save # of found records prior to cleanup
    JOIN_TAB *sort_tab;
    JOIN_TAB *join_tab= join->join_tab;
    uint const_tables= join->const_tables;

    // Take record count from first non constant table or from last tmp table
    if (join->aggr_tables > 0)
      sort_tab= join_tab + join->top_join_tab_count + join->aggr_tables - 1;
    else
    {
      DBUG_ASSERT(!join->only_const_tables());
      sort_tab= join_tab + const_tables;
    }
    if (sort_tab->filesort &&
        join->select_options & OPTION_FOUND_ROWS &&
        sort_tab->filesort->sortorder &&
        sort_tab->filesort->limit != HA_POS_ERROR)
    {
      join->thd->limit_found_rows= sort_tab->records;
    }
  }

  {
    /*
      The following will unlock all cursors if the command wasn't an
      update command
    */
    join->join_free();			// Unlock all cursors
  }
  if (error == NESTED_LOOP_OK)
  {
    /*
      Sic: this branch works even if rc != 0, e.g. when
      send_data above returns an error.
    */
    if (join->result->send_eof())
      rc= 1;                                  // Don't send error
    DBUG_PRINT("info",("%ld records output", (long) join->send_records));
  }
  else
    rc= -1;
#ifndef DBUG_OFF
  if (rc)
  {
    DBUG_PRINT("error",("Error: do_select() failed"));
  }
#endif
  rc= join->thd->is_error() ? -1 : rc;
  DBUG_RETURN(rc);
}