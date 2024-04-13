int Arg_comparator::compare_e_decimal()
{
  VDec val1(*a), val2(*b);
  if (val1.is_null() || val2.is_null())
    return MY_TEST(val1.is_null() && val2.is_null());
  val1.round_self_if_needed((*a)->decimals, HALF_UP);
  val2.round_self_if_needed((*b)->decimals, HALF_UP);
  return MY_TEST(val1.cmp(val2) == 0);
}