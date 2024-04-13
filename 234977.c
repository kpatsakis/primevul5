  void used_tables_and_const_cache_update_and_join(Item *item)
  {
    item->update_used_tables();
    used_tables_and_const_cache_join(item);
  }