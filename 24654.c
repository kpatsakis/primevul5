Item *Item_cond::build_clone(THD *thd)
{
  List_iterator_fast<Item> li(list);
  Item *item;
  Item_cond *copy= (Item_cond *) get_copy(thd);
  if (!copy)
    return 0;
  copy->list.empty();
  while ((item= li++))
  {
    Item *arg_clone= item->build_clone(thd);
    if (!arg_clone)
      return 0;
    if (copy->list.push_back(arg_clone, thd->mem_root))
      return 0;
  }
  return copy;
}