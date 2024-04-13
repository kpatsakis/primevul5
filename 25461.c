bool Item_func_in::fix_for_row_comparison_using_bisection(THD *thd)
{
  if (unlikely(!(array= new (thd->mem_root) in_row(thd, arg_count-1, 0))))
    return true;
  cmp_item_row *cmp= &((in_row*)array)->tmp;
  if (cmp->prepare_comparators(thd, func_name(), this, 0))
    return true;
  fix_in_vector();
  return false;
}