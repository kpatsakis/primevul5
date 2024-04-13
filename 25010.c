int Arg_comparator::compare_e_string()
{
  String *res1,*res2;
  res1= (*a)->val_str(&value1);
  res2= (*b)->val_str(&value2);
  if (!res1 || !res2)
    return MY_TEST(res1 == res2);
  return MY_TEST(sortcmp(res1, res2, compare_collation()) == 0);
}