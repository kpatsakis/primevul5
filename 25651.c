Item_func_ne::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                             uint *and_level, table_map usable_tables,
                             SARGABLE_PARAM **sargables)
{
  if (!(used_tables() & OUTER_REF_TABLE_BIT))
  {
    /*
      QQ: perhaps test for !is_local_field(args[1]) is not really needed here.
      Other comparison functions, e.g. Item_func_le, Item_func_gt, etc,
      do not have this test. See Item_bool_func2::add_key_field_optimize_op().
      Check with the optimizer team.
    */
    if (is_local_field(args[0]) && !is_local_field(args[1]))
      add_key_equal_fields(join, key_fields, *and_level, this,
                           (Item_field*) (args[0]->real_item()), false,
                           &args[1], 1, usable_tables, sargables);
    /*
      QQ: perhaps test for !is_local_field(args[0]) is not really needed here.
    */
    if (is_local_field(args[1]) && !is_local_field(args[0]))
      add_key_equal_fields(join, key_fields, *and_level, this,
                           (Item_field*) (args[1]->real_item()), false,
                           &args[0], 1, usable_tables, sargables);
  }
}