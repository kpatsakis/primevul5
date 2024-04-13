int Arg_comparator::compare_int_signed()
{
  longlong val1= (*a)->val_int();
  if (!(*a)->null_value)
  {
    longlong val2= (*b)->val_int();
    if (!(*b)->null_value)
      return compare_not_null_values(val1, val2);
  }
  if (set_null)
    owner->null_value= 1;
  return -1;
}