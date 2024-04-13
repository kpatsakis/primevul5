  Item *get_copy(THD *thd)
  { return get_item_copy<Item_copy_timestamp>(thd, this); }