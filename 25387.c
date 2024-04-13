bool Item_in_optimizer::eval_not_null_tables(void *opt_arg)
{
  not_null_tables_cache= 0;
  if (is_top_level_item())
  {
    /*
      It is possible to determine NULL-rejectedness of the left arguments
      of IN only if it is a top-level predicate.
    */
    not_null_tables_cache= args[0]->not_null_tables();
  }
  return FALSE;
}