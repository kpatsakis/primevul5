longlong Item_func_case::int_op()
{
  DBUG_ASSERT(fixed == 1);
  Item *item= find_item();
  longlong res;

  if (!item)
  {
    null_value=1;
    return 0;
  }
  res=item->val_int();
  null_value=item->null_value;
  return res;
}