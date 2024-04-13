inline bool TABLE::mark_virtual_column_with_deps(Field *field)
{
  bool res;
  DBUG_ASSERT(field->vcol_info);
  if (!(res= bitmap_fast_test_and_set(read_set, field->field_index)))
    mark_virtual_column_deps(field);
  return res;
}