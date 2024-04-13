  Item *get_copy(THD *thd, MEM_ROOT *mem_root)
  { return get_item_copy<Item_ref_null_helper>(thd, mem_root, this); }