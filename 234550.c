Field *Item_sum::create_tmp_field(bool group, TABLE *table)
{
  Field *UNINIT_VAR(new_field);
  MEM_ROOT *mem_root= table->in_use->mem_root;

  switch (cmp_type()) {
  case REAL_RESULT:
  {
    new_field= new (mem_root)
      Field_double(max_char_length(), maybe_null, &name, decimals, TRUE);
    break;
  }
  case INT_RESULT:
  case TIME_RESULT:
  case DECIMAL_RESULT:
  case STRING_RESULT:
    new_field= tmp_table_field_from_field_type(table);
    break;
  case ROW_RESULT:
    // This case should never be choosen
    DBUG_ASSERT(0);
    new_field= 0;
    break;
  }
  if (new_field)
    new_field->init(table);
  return new_field;
}