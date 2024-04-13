bool setup_on_expr(THD *thd, TABLE_LIST *table, bool is_update)
{
  uchar buff[STACK_BUFF_ALLOC];			// Max argument in function
  if (check_stack_overrun(thd, STACK_MIN_SIZE, buff))
    return TRUE;				// Fatal error flag is set!
  for(; table; table= table->next_local)
  {
    TABLE_LIST *embedded; /* The table at the current level of nesting. */
    TABLE_LIST *embedding= table; /* The parent nested table reference. */
    do
    {
      embedded= embedding;
      if (embedded->on_expr)
      {
        thd->where="on clause";
        embedded->on_expr->mark_as_condition_AND_part(embedded);
        if ((!embedded->on_expr->fixed &&
             embedded->on_expr->fix_fields(thd, &embedded->on_expr)) ||
            embedded->on_expr->check_cols(1))
          return TRUE;
      }
      /*
        If it's a semi-join nest, fix its "left expression", as it is used by
        the SJ-Materialization
      */
      if (embedded->sj_subq_pred)
      {
        Item **left_expr= &embedded->sj_subq_pred->left_expr;
        if (!(*left_expr)->fixed && (*left_expr)->fix_fields(thd, left_expr))
          return TRUE;
      }

      embedding= embedded->embedding;
    }
    while (embedding &&
           embedding->nested_join->join_list.head() == embedded);

    if (table->is_merged_derived())
    {
      SELECT_LEX *select_lex= table->get_single_select();
      setup_on_expr(thd, select_lex->get_table_list(), is_update);
    }

    /* process CHECK OPTION */
    if (is_update)
    {
      TABLE_LIST *view= table->top_table();
      if (view->effective_with_check)
      {
        if (view->prepare_check_option(thd))
          return TRUE;
        thd->change_item_tree(&table->check_option, view->check_option);
      }
    }
  }
  return FALSE;
}