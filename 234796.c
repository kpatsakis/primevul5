void JOIN::cleanup_item_list(List<Item> &items) const
{
  DBUG_ENTER("JOIN::cleanup_item_list");
  if (!items.is_empty())
  {
    List_iterator_fast<Item> it(items);
    Item *item;
    while ((item= it++))
      item->cleanup();
  }
  DBUG_VOID_RETURN;
}