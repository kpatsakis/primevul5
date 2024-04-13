bool Arg_comparator::set_cmp_func_real()
{
  if ((((*a)->result_type() == DECIMAL_RESULT && !(*a)->const_item() &&
        (*b)->result_type() == STRING_RESULT  &&  (*b)->const_item()) ||
      ((*b)->result_type() == DECIMAL_RESULT && !(*b)->const_item() &&
       (*a)->result_type() == STRING_RESULT  &&  (*a)->const_item())))
  {
    /*
     <non-const decimal expression> <cmp> <const string expression>
     or
     <const string expression> <cmp> <non-const decimal expression>

     Do comparison as decimal rather than float, in order not to lose precision.
    */
    m_compare_handler= &type_handler_newdecimal;
    return set_cmp_func_decimal();
  }

  THD *thd= current_thd;
  func= is_owner_equal_func() ? &Arg_comparator::compare_e_real :
                                &Arg_comparator::compare_real;
  if ((*a)->decimals < NOT_FIXED_DEC && (*b)->decimals < NOT_FIXED_DEC)
  {
    precision= 5 / log_10[MY_MAX((*a)->decimals, (*b)->decimals) + 1];
    if (func == &Arg_comparator::compare_real)
      func= &Arg_comparator::compare_real_fixed;
    else if (func == &Arg_comparator::compare_e_real)
      func= &Arg_comparator::compare_e_real_fixed;
  }
  a= cache_converted_constant(thd, a, &a_cache, compare_type_handler());
  b= cache_converted_constant(thd, b, &b_cache, compare_type_handler());
  return false;
}