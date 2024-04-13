Item_func_like::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                               uint *and_level, table_map usable_tables,
                               SARGABLE_PARAM **sargables)
{
  if (is_local_field(args[0]) && with_sargable_pattern())
  {
    /*
      SELECT * FROM t1 WHERE field LIKE const_pattern
      const_pattern starts with a non-wildcard character
    */
    add_key_equal_fields(join, key_fields, *and_level, this,
                         (Item_field*) args[0]->real_item(), false,
                         args + 1, 1, usable_tables, sargables);
  }
}