bool Item_cond::excl_dep_on_grouping_fields(st_select_lex *sel)
{
  if (has_rand_bit())
    return false;
  List_iterator_fast<Item> li(list);
  Item *item;
  while ((item= li++))
  {
    if (!item->excl_dep_on_grouping_fields(sel))
      return false;
  }
  return true;
}