void st_select_lex::update_correlated_cache()
{
  TABLE_LIST *tl;
  List_iterator<TABLE_LIST> ti(leaf_tables);

  is_correlated= false;

  while ((tl= ti++))
  {
    //    is_correlated|= tl->is_with_table_recursive_reference();
    if (tl->on_expr)
      is_correlated|= MY_TEST(tl->on_expr->used_tables() & OUTER_REF_TABLE_BIT);
    for (TABLE_LIST *embedding= tl->embedding ; embedding ;
         embedding= embedding->embedding)
    {
      if (embedding->on_expr)
        is_correlated|= MY_TEST(embedding->on_expr->used_tables() &
                                OUTER_REF_TABLE_BIT);
    }
  }

  if (join->conds)
    is_correlated|= MY_TEST(join->conds->used_tables() & OUTER_REF_TABLE_BIT);

  is_correlated|= join->having_is_correlated;

  if (join->having)
    is_correlated|= MY_TEST(join->having->used_tables() & OUTER_REF_TABLE_BIT);

  if (join->tmp_having)
    is_correlated|= MY_TEST(join->tmp_having->used_tables() &
                            OUTER_REF_TABLE_BIT);

  Item *item;
  List_iterator_fast<Item> it(join->fields_list);
  while ((item= it++))
    is_correlated|= MY_TEST(item->used_tables() & OUTER_REF_TABLE_BIT);

  for (ORDER *order= group_list.first; order; order= order->next)
    is_correlated|= MY_TEST((*order->item)->used_tables() &
                            OUTER_REF_TABLE_BIT);

  if (!master_unit()->is_union())
  {
    for (ORDER *order= order_list.first; order; order= order->next)
      is_correlated|= MY_TEST((*order->item)->used_tables() &
                              OUTER_REF_TABLE_BIT);
  }

  if (!is_correlated)
    uncacheable&= ~UNCACHEABLE_DEPENDENT;
}