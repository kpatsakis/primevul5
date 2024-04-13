void Item_equal::update_used_tables()
{
  not_null_tables_cache= used_tables_cache= 0;
  if ((const_item_cache= cond_false || cond_true))
    return;
  Item_equal_fields_iterator it(*this);
  Item *item;
  const_item_cache= 1;
  while ((item= it++))
  {
    item->update_used_tables();
    used_tables_cache|= item->used_tables();
    /* see commentary at Item_equal::update_const() */
    const_item_cache&= item->const_item() && !item->is_outer_field();
  }
}