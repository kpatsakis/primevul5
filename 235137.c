  Item *get_copy(THD *thd, MEM_ROOT *mem_root)
  { return get_item_copy<Item_time_literal>(thd, mem_root, this); }