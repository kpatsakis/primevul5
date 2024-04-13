Item_func_null_predicate::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                                         uint *and_level,
                                         table_map usable_tables,
                                         SARGABLE_PARAM **sargables)
{
  /* column_name IS [NOT] NULL */
  if (is_local_field(args[0]) && !(used_tables() & OUTER_REF_TABLE_BIT))
  {
    Item *tmp= new (join->thd->mem_root) Item_null(join->thd);
    if (unlikely(!tmp))                       // Should never be true
      return;
    add_key_equal_fields(join, key_fields, *and_level, this,
                         (Item_field*) args[0]->real_item(),
                         functype() == Item_func::ISNULL_FUNC,
                         &tmp, 1, usable_tables, sargables);
  }
}