bool Item_func_eq::check_equality(THD *thd, COND_EQUAL *cond_equal,
                                  List<Item> *eq_list)
{
  Item *left_item= arguments()[0];
  Item *right_item= arguments()[1];

  if (left_item->type() == Item::ROW_ITEM &&
      right_item->type() == Item::ROW_ITEM)
  {
    return check_row_equality(thd,
                              cmp.subcomparators(),
                              (Item_row *) left_item,
                              (Item_row *) right_item,
                              cond_equal, eq_list);
  }
  return check_simple_equality(thd,
                               Context(ANY_SUBST,
                                       compare_type(),
                                       compare_collation()),
                               left_item, right_item, cond_equal);
}