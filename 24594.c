Item_func_nullif::real_op()
{
  DBUG_ASSERT(fixed == 1);
  double value;
  if (!compare())
  {
    null_value=1;
    return 0.0;
  }
  value= args[2]->val_real();
  null_value= args[2]->null_value;
  return value;
}