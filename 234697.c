Item_func_in::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                             uint *and_level, table_map usable_tables,
                             SARGABLE_PARAM **sargables)
{
  if (is_local_field(args[0]) && !(used_tables() & OUTER_REF_TABLE_BIT))
  {
    DBUG_ASSERT(arg_count != 2);
    add_key_equal_fields(join, key_fields, *and_level, this,
                         (Item_field*) (args[0]->real_item()), false,
                         args + 1, arg_count - 1, usable_tables, sargables);
  }
  else if (key_item()->type() == Item::ROW_ITEM &&
           !(used_tables() & OUTER_REF_TABLE_BIT))
  {
    Item_row *key_row= (Item_row *) key_item();
    Item **key_col= key_row->addr(0);
    uint row_cols= key_row->cols();
    for (uint i= 0; i < row_cols; i++, key_col++)
    {
      if (is_local_field(*key_col))
      {
        Item_field *field_item= (Item_field *)((*key_col)->real_item());
        add_key_equal_fields(join, key_fields, *and_level, this,
                             field_item, false, args + 1, arg_count - 1,
                             usable_tables, sargables, i + 1);
      } 
    }
  }
  
}