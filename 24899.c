  Item *get_copy(THD *thd)
  { return get_item_copy<Item_timestamp_literal>(thd, this); }