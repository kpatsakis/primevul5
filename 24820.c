bool Arg_comparator::set_cmp_func_decimal()
{
  THD *thd= current_thd;
  func= is_owner_equal_func() ? &Arg_comparator::compare_e_decimal :
                                &Arg_comparator::compare_decimal;
  a= cache_converted_constant(thd, a, &a_cache, compare_type_handler());
  b= cache_converted_constant(thd, b, &b_cache, compare_type_handler());
  return false;
}