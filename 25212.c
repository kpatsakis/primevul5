my_decimal *Item_func_case::decimal_op(my_decimal *decimal_value)
{
  DBUG_ASSERT(fixed == 1);
  Item *item= find_item();
  my_decimal *res;

  if (!item)
  {
    null_value=1;
    return 0;
  }

  res= item->val_decimal(decimal_value);
  null_value= item->null_value;
  return res;
}