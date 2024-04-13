bool Item_equal::fix_length_and_dec()
{
  Item *item= get_first(NO_PARTICULAR_TAB, NULL);
  const Type_handler *handler= item->type_handler();
  eval_item= handler->make_cmp_item(current_thd, item->collation.collation);
  return eval_item == NULL;
}