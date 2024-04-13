longlong Item_func_between::val_int_cmp_real()
{
  double value= args[0]->val_real(),a,b;
  if ((null_value=args[0]->null_value))
    return 0;					/* purecov: inspected */
  a= args[1]->val_real();
  b= args[2]->val_real();
  if (!args[1]->null_value && !args[2]->null_value)
    return (longlong) ((value >= a && value <= b) != negated);
  if (args[1]->null_value && args[2]->null_value)
    null_value= true;
  else if (args[1]->null_value)
  {
    null_value= value <= b;			// not null if false range.
  }
  else
  {
    null_value= value >= a;
  }
  return (longlong) (!null_value && negated);
}