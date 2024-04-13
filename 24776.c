longlong Item_func_regexp_instr::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if ((null_value= re.recompile(args[1])))
    return 0;

  if ((null_value= re.exec(args[0], 0, 1)))
    return 0;

  return re.match() ? re.subpattern_start(0) + 1 : 0;
}