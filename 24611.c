  Item *get_copy(THD *thd)
  { return get_item_copy<Item_direct_ref>(thd, this); }