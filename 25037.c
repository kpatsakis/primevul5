int Arg_comparator::compare_time()
{
  THD *thd= current_thd;
  longlong val1= (*a)->val_time_packed(thd);
  if (!(*a)->null_value)
  {
    longlong val2= (*b)->val_time_packed(thd);
    if (!(*b)->null_value)
      return compare_not_null_values(val1, val2);
  }
  if (set_null)
    owner->null_value= true;
  return -1;
}