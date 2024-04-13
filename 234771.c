add_key_equal_fields(JOIN *join, KEY_FIELD **key_fields, uint and_level,
                     Item_bool_func *cond, Item *field_item,
                     bool eq_func, Item **val,
                     uint num_values, table_map usable_tables,
                     SARGABLE_PARAM **sargables, uint row_col_no= 0)
{
  Field *field= ((Item_field *) (field_item->real_item()))->field;
  add_key_field(join, key_fields, and_level, cond, field,
                eq_func, val, num_values, usable_tables, sargables,
                row_col_no);
  Item_equal *item_equal= field_item->get_item_equal();
  if (item_equal)
  { 
    /*
      Add to the set of possible key values every substitution of
      the field for an equal field included into item_equal
    */
    Item_equal_fields_iterator it(*item_equal);
    while (it++)
    {
      Field *equal_field= it.get_curr_field();
      if (!field->eq(equal_field))
      {
        add_key_field(join, key_fields, and_level, cond, equal_field,
                      eq_func, val, num_values, usable_tables,
                      sargables, row_col_no);
      }
    }
  }
}