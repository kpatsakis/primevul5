  Item *get_copy(THD *thd)
  { return get_item_copy<Item_hex_string>(thd, this); }