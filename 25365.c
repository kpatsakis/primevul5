  Item *get_copy(THD *thd)
  { return get_item_copy<Item_cache_str_for_nullif>(thd, this); }