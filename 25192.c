void Item_equal::merge_into_list(THD *thd, List<Item_equal> *list,
                                 bool save_merged,
                                 bool only_intersected)
{
  Item_equal *item;
  List_iterator<Item_equal> it(*list);
  Item_equal *merge_into= NULL;
  while((item= it++))
  {
    if (!merge_into)
    {
      if (item->merge_with_check(thd, this, save_merged))
        merge_into= item;
    }
    else
    {
      if (merge_into->merge_with_check(thd, item, false))
        it.remove();
    }
  }
  if (!only_intersected && !merge_into)
    list->push_back(this, thd->mem_root);
}