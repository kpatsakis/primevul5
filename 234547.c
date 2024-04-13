void JOIN::cache_const_exprs()
{
  bool cache_flag= FALSE;
  bool *analyzer_arg= &cache_flag;

  /* No need in cache if all tables are constant. */
  if (const_tables == table_count)
    return;

  if (conds)
    conds->compile(thd, &Item::cache_const_expr_analyzer, (uchar **)&analyzer_arg,
                  &Item::cache_const_expr_transformer, (uchar *)&cache_flag);
  cache_flag= FALSE;
  if (having)
    having->compile(thd, &Item::cache_const_expr_analyzer, (uchar **)&analyzer_arg,
                    &Item::cache_const_expr_transformer, (uchar *)&cache_flag);

  for (JOIN_TAB *tab= first_depth_first_tab(this); tab;
       tab= next_depth_first_tab(this, tab))
  {
    if (*tab->on_expr_ref)
    {
      cache_flag= FALSE;
      (*tab->on_expr_ref)->compile(thd, &Item::cache_const_expr_analyzer,
                                 (uchar **)&analyzer_arg,
                                 &Item::cache_const_expr_transformer,
                                 (uchar *)&cache_flag);
    }
  }
}