  Item *get_copy(THD *thd)
  { return get_item_copy<Item_field>(thd, this); }