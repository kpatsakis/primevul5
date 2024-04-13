Item_func_nullif::str_op(String *str)
{
  DBUG_ASSERT(fixed == 1);
  String *res;
  if (!compare())
  {
    null_value=1;
    return 0;
  }
  res= args[2]->val_str(str);
  null_value= args[2]->null_value;
  return res;
}