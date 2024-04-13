bool Item_func_case::aggregate_then_and_else_arguments(THD *thd, uint start)
{
  if (aggregate_for_result(func_name(), args + start, arg_count - start, true))
    return true;

  if (fix_attributes(args + start, arg_count - start))
    return true;

  return false;
}