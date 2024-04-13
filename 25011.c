int Arg_comparator::compare_native()
{
  THD *thd= current_thd;
  if (!(*a)->val_native_with_conversion(thd, &m_native1,
                                        compare_type_handler()))
  {
    if (!(*b)->val_native_with_conversion(thd, &m_native2,
                                          compare_type_handler()))
    {
      if (set_null)
        owner->null_value= 0;
      return compare_type_handler()->cmp_native(m_native1, m_native2);
    }
  }
  if (set_null)
    owner->null_value= 1;
  return -1;
}