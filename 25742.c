bool mysql_multi_update(THD *thd, TABLE_LIST *table_list, List<Item> *fields,
                        List<Item> *values, COND *conds, ulonglong options,
                        enum enum_duplicates handle_duplicates,
                        bool ignore, SELECT_LEX_UNIT *unit,
                        SELECT_LEX *select_lex, multi_update **result)
{
  bool res;
  DBUG_ENTER("mysql_multi_update");

  if (!(*result= new (thd->mem_root) multi_update(thd, table_list,
                                 &thd->lex->select_lex.leaf_tables,
                                 fields, values, handle_duplicates, ignore)))
  {
    DBUG_RETURN(TRUE);
  }

  if ((*result)->init(thd))
    DBUG_RETURN(1);

  thd->abort_on_warning= !ignore && thd->is_strict_mode();
  List<Item> total_list;

  if (setup_tables(thd, &select_lex->context, &select_lex->top_join_list,
                   table_list, select_lex->leaf_tables, FALSE, FALSE))
    DBUG_RETURN(1);

  if (select_lex->vers_setup_conds(thd, table_list))
    DBUG_RETURN(1);

  res= mysql_select(thd,
                    table_list, select_lex->with_wild, total_list, conds,
                    select_lex->order_list.elements,
                    select_lex->order_list.first, NULL, NULL, NULL,
                    options | SELECT_NO_JOIN_CACHE | SELECT_NO_UNLOCK |
                    OPTION_SETUP_TABLES_DONE,
                    *result, unit, select_lex);

  DBUG_PRINT("info",("res: %d  report_error: %d", res, (int) thd->is_error()));
  res|= thd->is_error();
  if (unlikely(res))
    (*result)->abort_result_set();
  else
  {
    if (thd->lex->describe || thd->lex->analyze_stmt)
      res= thd->lex->explain->send_explain(thd);
  }
  thd->abort_on_warning= 0;
  DBUG_RETURN(res);
}