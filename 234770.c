bool Item_func_eq::check_equality(THD *thd, COND_EQUAL *cond_equal,
                                  List<Item> *eq_list)
{
  Item *left_item= arguments()[0];
  Item *right_item= arguments()[1];

  if (left_item->type() == Item::ROW_ITEM &&
      right_item->type() == Item::ROW_ITEM)
  {
    /*
      Item_splocal::type() for ROW variables returns Item::ROW_ITEM.
      Distinguish ROW-type Item_splocal from Item_row.
      Example query:
        SELECT 1 FROM DUAL WHERE row_sp_variable=ROW(100,200);
    */
    if (left_item->get_item_splocal() ||
        right_item->get_item_splocal())
      return false;
    return check_row_equality(thd,
                              cmp.subcomparators(),
                              (Item_row *) left_item,
                              (Item_row *) right_item,
                              cond_equal, eq_list);
  }
  return check_simple_equality(thd,
                               Context(ANY_SUBST,
                                       compare_type_handler(),
                                       compare_collation()),
                               left_item, right_item, cond_equal);
}