  void used_tables_and_const_cache_update_and_join(List<Item> &list)
  {
    List_iterator_fast<Item> li(list);
    Item *item;
    while ((item=li++))
      used_tables_and_const_cache_update_and_join(item);
  }