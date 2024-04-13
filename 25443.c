Item_func_nullif::int_op()
{
  DBUG_ASSERT(fixed == 1);
  longlong value;
  if (!compare())
  {
    null_value=1;
    return 0;
  }
  value= args[2]->val_int();
  null_value= args[2]->null_value;
  return value;
}