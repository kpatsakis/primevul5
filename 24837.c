inline bool TABLE::mark_column_with_deps(Field *field)
{
  bool res;
  if (!(res= bitmap_fast_test_and_set(read_set, field->field_index)))
  {
    if (field->vcol_info)
      mark_virtual_column_deps(field);
  }
  return res;
}