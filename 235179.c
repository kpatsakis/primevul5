  Item* get_copy(THD *thd, MEM_ROOT *mem_root)
  { return get_item_copy<Item_ident_for_show>(thd, mem_root, this); }