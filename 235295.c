  bool null_eq(const Item *item) const
  {
    DBUG_ASSERT(is_basic_value(NULL_ITEM));
    return item->type() == NULL_ITEM;
  }