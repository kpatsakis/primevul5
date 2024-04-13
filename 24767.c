bool Item_cond::excl_dep_on_table(table_map tab_map)
{
  if (used_tables() & OUTER_REF_TABLE_BIT)
    return false;
  if (!(used_tables() & ~tab_map))
    return true;
  List_iterator_fast<Item> li(list);
  Item *item;
  while ((item= li++))
  {
    if (!item->excl_dep_on_table(tab_map))
      return false;
  }
  return true;
}