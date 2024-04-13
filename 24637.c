Item_func_if::eval_not_null_tables(void *opt_arg)
{
  if (Item_func::eval_not_null_tables(NULL))
    return 1;

  not_null_tables_cache= (args[1]->not_null_tables() &
                          args[2]->not_null_tables());

  return 0;
}