bool Arg_comparator::set_cmp_func_int()
{
  THD *thd= current_thd;
  func= is_owner_equal_func() ? &Arg_comparator::compare_e_int :
                                &Arg_comparator::compare_int_signed;
  if ((*a)->field_type() == MYSQL_TYPE_YEAR &&
      (*b)->field_type() == MYSQL_TYPE_YEAR)
  {
    func= is_owner_equal_func() ? &Arg_comparator::compare_e_datetime :
                                  &Arg_comparator::compare_datetime;
  }
  else if (func == &Arg_comparator::compare_int_signed)
  {
    if ((*a)->unsigned_flag)
      func= (((*b)->unsigned_flag)?
             &Arg_comparator::compare_int_unsigned :
             &Arg_comparator::compare_int_unsigned_signed);
    else if ((*b)->unsigned_flag)
      func= &Arg_comparator::compare_int_signed_unsigned;
  }
  else if (func== &Arg_comparator::compare_e_int)
  {
    if ((*a)->unsigned_flag ^ (*b)->unsigned_flag)
      func= &Arg_comparator::compare_e_int_diff_signedness;
  }
  a= cache_converted_constant(thd, a, &a_cache, compare_type_handler());
  b= cache_converted_constant(thd, b, &b_cache, compare_type_handler());
  return false;
}