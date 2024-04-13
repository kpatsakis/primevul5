static void create_tmp_field_from_item_finalize(THD *thd,
                                                Field *new_field,
                                                Item *item,
                                                Item ***copy_func,
                                                bool modify_item)
{
  if (copy_func &&
      (item->is_result_field() ||
       (item->real_item()->is_result_field())))
    *((*copy_func)++) = item;			// Save for copy_funcs
  if (modify_item)
    item->set_result_field(new_field);
  if (item->type() == Item::NULL_ITEM)
    new_field->is_created_from_null_item= TRUE;
}