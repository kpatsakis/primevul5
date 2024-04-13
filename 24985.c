bool Item_bool_rowready_func2::fix_length_and_dec()
{
  max_length= 1;				     // Function returns 0 or 1

  /*
    As some compare functions are generated after sql_yacc,
    we have to check for out of memory conditions here
  */
  if (!args[0] || !args[1])
    return FALSE;
  return setup_args_and_comparator(current_thd, &cmp);
}