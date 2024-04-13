bool mysql_explain_union(THD *thd, SELECT_LEX_UNIT *unit, select_result *result)
{
  DBUG_ENTER("mysql_explain_union");
  bool res= 0;
  SELECT_LEX *first= unit->first_select();

  for (SELECT_LEX *sl= first; sl; sl= sl->next_select())
  {
    sl->set_explain_type(FALSE);
    sl->options|= SELECT_DESCRIBE;
  }

  if (unit->is_union() || unit->fake_select_lex)
  {
    ulonglong save_options= 0;

    if (unit->union_needs_tmp_table() && unit->fake_select_lex)
    {
      save_options= unit->fake_select_lex->options;
      unit->fake_select_lex->select_number= FAKE_SELECT_LEX_ID; // just for initialization
      unit->fake_select_lex->type= "UNION RESULT";
      unit->fake_select_lex->options|= SELECT_DESCRIBE;
    }
    if (!(res= unit->prepare(thd, result, SELECT_NO_UNLOCK | SELECT_DESCRIBE)))
      res= unit->exec();

    if (unit->union_needs_tmp_table() && unit->fake_select_lex)
      unit->fake_select_lex->options= save_options;
  }
  else
  {
    thd->lex->current_select= first;
    unit->set_limit(unit->global_parameters());
    res= mysql_select(thd, 
                      first->table_list.first,
                      first->with_wild, first->item_list,
                      first->where,
                      first->order_list.elements + first->group_list.elements,
                      first->order_list.first,
                      first->group_list.first,
                      first->having,
                      thd->lex->proc_list.first,
                      first->options | thd->variables.option_bits | SELECT_DESCRIBE,
                      result, unit, first);
  }
  DBUG_RETURN(res || thd->is_error());
}