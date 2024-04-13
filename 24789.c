double Item_func_case::real_op()
{
  DBUG_ASSERT(fixed == 1);
  Item *item= find_item();
  double res;

  if (!item)
  {
    null_value=1;
    return 0;
  }
  res= item->val_real();
  null_value=item->null_value;
  return res;
}