bool Item_func_between::eval_not_null_tables(void *opt_arg)
{
  if (Item_func_opt_neg::eval_not_null_tables(NULL))
    return 1;

  /* not_null_tables_cache == union(T1(e),T1(e1),T1(e2)) */
  if (pred_level && !negated)
    return 0;

  /* not_null_tables_cache == union(T1(e), intersection(T1(e1),T1(e2))) */
  not_null_tables_cache= (args[0]->not_null_tables() |
                          (args[1]->not_null_tables() &
                           args[2]->not_null_tables()));
  return 0;
}  