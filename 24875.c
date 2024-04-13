Item_func_nullif::is_null()
{
  return (null_value= (!compare() ? 1 : args[2]->is_null()));
}