static table_map get_table_map(List<Item> *items)
{
  List_iterator_fast<Item> item_it(*items);
  Item_field *item;
  table_map map= 0;

  while ((item= (Item_field *) item_it++))
    map|= item->all_used_tables();
  DBUG_PRINT("info", ("table_map: 0x%08lx", (long) map));
  return map;
}