  Item *get_copy(THD *thd)
  { return get_item_copy<Item_datetime_literal>(thd, this); }