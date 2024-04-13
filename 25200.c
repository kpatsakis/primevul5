void Item_cond::split_sum_func(THD *thd, Ref_ptr_array ref_pointer_array,
                               List<Item> &fields, uint flags)
{
  List_iterator<Item> li(list);
  Item *item;
  while ((item= li++))
    item->split_sum_func2(thd, ref_pointer_array, fields, li.ref(),
                          flags | SPLIT_SUM_SKIP_REGISTERED);
}