  Item *get_copy(THD *thd, MEM_ROOT *mem_root)
  { return get_item_copy<Item_cache_str_for_nullif>(thd, mem_root, this); }