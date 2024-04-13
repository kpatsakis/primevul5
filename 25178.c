bool Item_func_between::val_int_cmp_int_finalize(longlong value,
                                                 longlong a,
                                                 longlong b)
{
  if (!args[1]->null_value && !args[2]->null_value)
    return (longlong) ((value >= a && value <= b) != negated);
  if (args[1]->null_value && args[2]->null_value)
    null_value= true;
  else if (args[1]->null_value)
    null_value= value <= b;			// not null if false range.
  else
    null_value= value >= a;
  return (longlong) (!null_value && negated);
}