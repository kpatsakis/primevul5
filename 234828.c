static Field *create_tmp_field_from_item(THD *thd, Item *item, TABLE *table,
                                         Item ***copy_func, bool modify_item)
{
  DBUG_ASSERT(thd == table->in_use);
  Field* new_field= item->create_tmp_field(false, table);
  if (new_field)
    create_tmp_field_from_item_finalize(thd, new_field, item,
                                        copy_func, modify_item);
  return new_field;
}