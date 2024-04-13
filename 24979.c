bool Item_func_in::fix_for_row_comparison_using_cmp_items(THD *thd)
{
  if (make_unique_cmp_items(thd, cmp_collation.collation))
    return true;
  DBUG_ASSERT(get_comparator_type_handler(0) == &type_handler_row);
  DBUG_ASSERT(get_comparator_cmp_item(0));
  cmp_item_row *cmp_row= (cmp_item_row*) get_comparator_cmp_item(0);
  return cmp_row->prepare_comparators(thd, func_name(), this, 0);
}