Item_func_nullif::decimal_op(my_decimal * decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  my_decimal *res;
  if (!compare())
  {
    null_value=1;
    return 0;
  }
  res= args[2]->val_decimal(decimal_value);
  null_value= args[2]->null_value;
  return res;
}