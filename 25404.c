int Arg_comparator::compare_decimal()
{
  VDec val1(*a);
  if (!val1.is_null())
  {
    VDec val2(*b);
    if (!val2.is_null())
    {
      if (set_null)
        owner->null_value= 0;
      val1.round_self_if_needed((*a)->decimals, HALF_UP);
      val2.round_self_if_needed((*b)->decimals, HALF_UP);
      return val1.cmp(val2);
    }
  }
  if (set_null)
    owner->null_value= 1;
  return -1;
}