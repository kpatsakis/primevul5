longlong Item_func_between::val_int_cmp_int()
{
  Longlong_hybrid value= args[0]->to_longlong_hybrid();
  if ((null_value= args[0]->null_value))
    return 0;					/* purecov: inspected */
  Longlong_hybrid a= args[1]->to_longlong_hybrid();
  Longlong_hybrid b= args[2]->to_longlong_hybrid();
  if (!args[1]->null_value && !args[2]->null_value)
    return (longlong) ((value.cmp(a) >= 0 && value.cmp(b) <= 0) != negated);
  if (args[1]->null_value && args[2]->null_value)
    null_value= true;
  else if (args[1]->null_value)
    null_value= value.cmp(b) <= 0;              // not null if false range.
  else
    null_value= value.cmp(a) >= 0;
  return (longlong) (!null_value && negated);
}