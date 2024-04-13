longlong Item_func_between::val_int_cmp_string()
{
  String *value,*a,*b;
  value=args[0]->val_str(&value0);
  if ((null_value=args[0]->null_value))
    return 0;
  a= args[1]->val_str(&value1);
  b= args[2]->val_str(&value2);
  if (!args[1]->null_value && !args[2]->null_value)
    return (longlong) ((sortcmp(value,a,cmp_collation.collation) >= 0 &&
                        sortcmp(value,b,cmp_collation.collation) <= 0) !=
                       negated);
  if (args[1]->null_value && args[2]->null_value)
    null_value= true;
  else if (args[1]->null_value)
  {
    // Set to not null if false range.
    null_value= sortcmp(value,b,cmp_collation.collation) <= 0;
  }
  else
  {
    // Set to not null if false range.
    null_value= sortcmp(value,a,cmp_collation.collation) >= 0;
  }
  return (longlong) (!null_value && negated);
}