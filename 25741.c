void multi_update::update_used_tables()
{
  Item *item;
  List_iterator_fast<Item> it(*values);
  while ((item= it++))
  {
    item->update_used_tables();
  }
}