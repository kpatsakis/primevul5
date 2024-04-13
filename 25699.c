bool handle_select(THD *thd, LEX *lex, select_result *result,
                   ulong setup_tables_done_option)
{
  bool res;
  SELECT_LEX *select_lex = &lex->select_lex;
  DBUG_ENTER("handle_select");
  MYSQL_SELECT_START(thd->query());

  if (select_lex->master_unit()->is_union() || 
      select_lex->master_unit()->fake_select_lex)
    res= mysql_union(thd, lex, result, &lex->unit, setup_tables_done_option);
  else
  {
    SELECT_LEX_UNIT *unit= &lex->unit;
    unit->set_limit(unit->global_parameters());
    /*
      'options' of mysql_select will be set in JOIN, as far as JOIN for
      every PS/SP execution new, we will not need reset this flag if 
      setup_tables_done_option changed for next rexecution
    */
    res= mysql_select(thd,
		      select_lex->table_list.first,
		      select_lex->with_wild, select_lex->item_list,
		      select_lex->where,
		      select_lex->order_list.elements +
		      select_lex->group_list.elements,
		      select_lex->order_list.first,
		      select_lex->group_list.first,
		      select_lex->having,
		      lex->proc_list.first,
		      select_lex->options | thd->variables.option_bits |
                      setup_tables_done_option,
		      result, unit, select_lex);
  }
  DBUG_PRINT("info",("res: %d  report_error: %d", res,
		     thd->is_error()));
  res|= thd->is_error();
  if (unlikely(res))
    result->abort_result_set();
  if (thd->killed == ABORT_QUERY)
  {
    /*
      If LIMIT ROWS EXAMINED interrupted query execution, issue a warning,
      continue with normal processing and produce an incomplete query result.
    */
    bool saved_abort_on_warning= thd->abort_on_warning;
    thd->abort_on_warning= false;
    push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                        ER_QUERY_EXCEEDED_ROWS_EXAMINED_LIMIT,
                        ER_THD(thd, ER_QUERY_EXCEEDED_ROWS_EXAMINED_LIMIT),
                        thd->accessed_rows_and_keys,
                        thd->lex->limit_rows_examined->val_uint());
    thd->abort_on_warning= saved_abort_on_warning;
    thd->reset_killed();
  }
  /* Disable LIMIT ROWS EXAMINED after query execution. */
  thd->lex->limit_rows_examined_cnt= ULONGLONG_MAX;

  MYSQL_SELECT_DONE((int) res, (ulong) thd->limit_found_rows);
  DBUG_RETURN(res);
}