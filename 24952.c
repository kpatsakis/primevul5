bool Item_equal::fix_fields(THD *thd, Item **ref)
{ 
  DBUG_ASSERT(fixed == 0);
  Item_equal_fields_iterator it(*this);
  Item *item;
  Field *first_equal_field= NULL;
  Field *last_equal_field= NULL;
  Field *prev_equal_field= NULL;
  not_null_tables_cache= used_tables_cache= 0;
  const_item_cache= 0;
  while ((item= it++))
  {
    table_map tmp_table_map;
    used_tables_cache|= item->used_tables();
    tmp_table_map= item->not_null_tables();
    not_null_tables_cache|= tmp_table_map;
    DBUG_ASSERT(!item->with_sum_func() && !item->with_subquery());
    if (item->maybe_null)
      maybe_null= 1;
    if (!item->get_item_equal())
      item->set_item_equal(this);
    if (link_equal_fields && item->real_item()->type() == FIELD_ITEM)
    {
      last_equal_field= ((Item_field *) (item->real_item()))->field;
      if (!prev_equal_field)
        first_equal_field= last_equal_field;
      else
        prev_equal_field->next_equal_field= last_equal_field;
      prev_equal_field= last_equal_field;         
    }
  }
  if (prev_equal_field && last_equal_field != first_equal_field)
    last_equal_field->next_equal_field= first_equal_field;
  if (fix_length_and_dec())
    return TRUE;
  fixed= 1;
  return FALSE;
}