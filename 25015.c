bool Item_func_equal::fix_length_and_dec()
{
  bool rc= Item_bool_rowready_func2::fix_length_and_dec();
  maybe_null=null_value=0;
  return rc;
}