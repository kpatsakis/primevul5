  Item *get_copy(THD *thd)
  { return get_item_copy<Item_direct_view_ref>(thd, this); }