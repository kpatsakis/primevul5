void Item_func_nullif::split_sum_func(THD *thd, Ref_ptr_array ref_pointer_array,
                                      List<Item> &fields, uint flags)
{
  if (m_cache)
  {
    flags|= SPLIT_SUM_SKIP_REGISTERED; // See Item_func::split_sum_func
    m_cache->split_sum_func2_example(thd, ref_pointer_array, fields, flags);
    args[1]->split_sum_func2(thd, ref_pointer_array, fields, &args[1], flags);
  }
  else
  {
    Item_func::split_sum_func(thd, ref_pointer_array, fields, flags);
  }
}