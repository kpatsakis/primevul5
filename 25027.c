bool Item_equal::walk(Item_processor processor, bool walk_subquery, void *arg)
{
  Item *item;
  Item_equal_fields_iterator it(*this);
  while ((item= it++))
  {
    if (item->walk(processor, walk_subquery, arg))
      return 1;
  }
  return Item_func::walk(processor, walk_subquery, arg);
}