  bool int_eq(longlong value, const Item *item) const
  {
    DBUG_ASSERT(is_basic_value(INT_ITEM));
    return is_basic_value(item, INT_ITEM) &&
           value == ((Item_basic_value*)item)->val_int() &&
           (value >= 0 || item->unsigned_flag == unsigned_flag);
  }