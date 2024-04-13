  void used_tables_and_const_cache_join(const Item *item)
  {
    used_tables_cache|= item->used_tables();
    const_item_cache&= item->const_item();
  }