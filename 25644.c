static Field *create_tmp_field_from_item(THD *thd, Item *item, TABLE *table,
                                         Item ***copy_func, bool modify_item)
{
  DBUG_ASSERT(thd == table->in_use);
  Field *new_field= item->Item::create_tmp_field(false, table);

  if (copy_func &&
      (item->is_result_field() || 
       (item->real_item()->is_result_field())))
    *((*copy_func)++) = item;			// Save for copy_funcs
  if (modify_item)
    item->set_result_field(new_field);
  if (item->type() == Item::NULL_ITEM)
    new_field->is_created_from_null_item= TRUE;
  return new_field;
}