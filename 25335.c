longlong Item_func_regex::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if ((null_value= re.recompile(args[1])))
    return 0;

  if ((null_value= re.exec(args[0], 0, 0)))
    return 0;

  return re.match();
}