int Arg_comparator::compare_e_datetime()
{
  THD *thd= current_thd;
  longlong val1= (*a)->val_datetime_packed(thd);
  longlong val2= (*b)->val_datetime_packed(thd);
  if ((*a)->null_value || (*b)->null_value)
    return MY_TEST((*a)->null_value && (*b)->null_value);
  return MY_TEST(val1 == val2);
}