int Arg_comparator::compare_e_native()
{
  THD *thd= current_thd;
  bool res1= (*a)->val_native_with_conversion(thd, &m_native1,
                                              compare_type_handler());
  bool res2= (*b)->val_native_with_conversion(thd, &m_native2,
                                              compare_type_handler());
  if (res1 || res2)
    return MY_TEST(res1 == res2);
  return MY_TEST(compare_type_handler()->cmp_native(m_native1, m_native2) == 0);
}