static bool check_row_equality(THD *thd, const Arg_comparator *comparators,
                               Item *left_row, Item_row *right_row,
                               COND_EQUAL *cond_equal, List<Item>* eq_list)
{ 
  uint n= left_row->cols();
  for (uint i= 0 ; i < n; i++)
  {
    bool is_converted;
    Item *left_item= left_row->element_index(i);
    Item *right_item= right_row->element_index(i);
    if (left_item->type() == Item::ROW_ITEM &&
        right_item->type() == Item::ROW_ITEM)
    {
      is_converted= check_row_equality(thd,
                                       comparators[i].subcomparators(),
                                       (Item_row *) left_item,
                                       (Item_row *) right_item,
			               cond_equal, eq_list);
    }
    else
    { 
      const Arg_comparator *tmp= &comparators[i];
      is_converted= check_simple_equality(thd,
                                          Item::Context(Item::ANY_SUBST,
                                                        tmp->compare_type(),
                                                  tmp->compare_collation()),
                                          left_item, right_item,
                                          cond_equal);
    }  
 
    if (!is_converted)
    {
      Item_func_eq *eq_item;
      if (!(eq_item= new (thd->mem_root) Item_func_eq(thd, left_item, right_item)) ||
          eq_item->set_cmp_func())
        return FALSE;
      eq_item->quick_fix_field();
      eq_list->push_back(eq_item, thd->mem_root);
    }
  }
  return TRUE;
}