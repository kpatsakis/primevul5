  Item *get_copy(THD *thd)
  { return get_item_copy<Item_time_literal>(thd, this); }