Item_bool_func2::add_key_fields_optimize_op(JOIN *join, KEY_FIELD **key_fields,
                                            uint *and_level,
                                            table_map usable_tables,
                                            SARGABLE_PARAM **sargables,
                                            bool equal_func)
{
  /* If item is of type 'field op field/constant' add it to key_fields */
  if (is_local_field(args[0]))
  {
    add_key_equal_fields(join, key_fields, *and_level, this,
                         (Item_field*) args[0]->real_item(), equal_func,
                         args + 1, 1, usable_tables, sargables);
  }
  if (is_local_field(args[1]))
  {
    add_key_equal_fields(join, key_fields, *and_level, this, 
                         (Item_field*) args[1]->real_item(), equal_func,
                         args, 1, usable_tables, sargables);
  }
}