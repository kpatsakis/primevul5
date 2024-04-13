bool Item_func_in::fix_for_scalar_comparison_using_cmp_items(THD *thd,
                                                             uint found_types)
{
  if (found_types & (1U << STRING_RESULT) &&
      agg_arg_charsets_for_comparison(cmp_collation, args, arg_count))
    return true;
  if (make_unique_cmp_items(thd, cmp_collation.collation))
    return true;
  return false;
}