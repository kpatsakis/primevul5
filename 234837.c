  bool real_eq(double value, const Item *item) const
  {
    DBUG_ASSERT(is_basic_value(REAL_ITEM));
    return is_basic_value(item, REAL_ITEM) &&
           value == ((Item_basic_value*)item)->val_real();
  }