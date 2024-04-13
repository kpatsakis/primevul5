bool Item_func_in::list_contains_null()
{
  Item **arg,**arg_end;
  for (arg= args + 1, arg_end= args+arg_count; arg != arg_end ; arg++)
  {
    if ((*arg)->null_inside())
      return 1;
  }
  return 0;
}