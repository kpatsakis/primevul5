bool Arg_comparator::set_cmp_func_string()
{
  THD *thd= current_thd;
  func= is_owner_equal_func() ? &Arg_comparator::compare_e_string :
                                &Arg_comparator::compare_string;
  if (compare_type() == STRING_RESULT &&
      (*a)->result_type() == STRING_RESULT &&
      (*b)->result_type() == STRING_RESULT)
  {
    /*
      We must set cmp_collation here as we may be called from for an automatic
      generated item, like in natural join
    */
    if (owner->agg_arg_charsets_for_comparison(&m_compare_collation, a, b))
      return true;

    if ((*a)->type() == Item::FUNC_ITEM &&
        ((Item_func *) (*a))->functype() == Item_func::JSON_EXTRACT_FUNC)
    {
      func= is_owner_equal_func() ? &Arg_comparator::compare_e_json_str:
                                    &Arg_comparator::compare_json_str;
      return 0;
    }
    else if ((*b)->type() == Item::FUNC_ITEM &&
             ((Item_func *) (*b))->functype() == Item_func::JSON_EXTRACT_FUNC)
    {
      func= is_owner_equal_func() ? &Arg_comparator::compare_e_json_str:
                                    &Arg_comparator::compare_str_json;
      return 0;
    }
  }

  a= cache_converted_constant(thd, a, &a_cache, compare_type_handler());
  b= cache_converted_constant(thd, b, &b_cache, compare_type_handler());
  return false;
}