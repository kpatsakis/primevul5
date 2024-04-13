bool Arg_comparator::set_cmp_func_time()
{
  THD *thd= current_thd;
  m_compare_collation= &my_charset_numeric;
  func= is_owner_equal_func() ? &Arg_comparator::compare_e_time :
                                &Arg_comparator::compare_time;
  a= cache_converted_constant(thd, a, &a_cache, compare_type_handler());
  b= cache_converted_constant(thd, b, &b_cache, compare_type_handler());
  return false;
}