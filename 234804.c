void count_cond_for_nj(SELECT_LEX *sel, TABLE_LIST *nested_join_table)
{
  List_iterator<TABLE_LIST> li(nested_join_table->nested_join->join_list);
  List_iterator<TABLE_LIST> li2(nested_join_table->nested_join->join_list);
  bool have_another = FALSE;
  TABLE_LIST *table;

  while ((table= li++) || (have_another && (li=li2, have_another=FALSE,
                                            (table= li++))))
  if (table->nested_join)
  {
    if (!table->on_expr)
    {
      /* It's a semi-join nest. Walk into it as if it wasn't a nest */
      have_another= TRUE;
      li2= li;
      li= List_iterator<TABLE_LIST>(table->nested_join->join_list); 
    }
    else
      count_cond_for_nj(sel, table); 
  }
  if (nested_join_table->on_expr)
    nested_join_table->on_expr->walk(&Item::count_sargable_conds, 0, sel);
    
}