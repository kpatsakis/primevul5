longlong Item_func_strcmp::val_int()
{
  DBUG_ASSERT(fixed == 1);
  String *a= args[0]->val_str(&value1);
  String *b= args[1]->val_str(&value2);
  if (!a || !b)
  {
    null_value=1;
    return 0;
  }
  int value= cmp_collation.sortcmp(a, b);
  null_value=0;
  return !value ? 0 : (value < 0 ? (longlong) -1 : (longlong) 1);
}