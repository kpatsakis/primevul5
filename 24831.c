String *Item_func_case::str_op(String *str)
{
  DBUG_ASSERT(fixed == 1);
  String *res;
  Item *item= find_item();

  if (!item)
  {
    null_value=1;
    return 0;
  }
  null_value= 0;
  if (!(res=item->val_str(str)))
    null_value= 1;
  return res;
}