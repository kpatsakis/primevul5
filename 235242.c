  bool str_eq(const String *value, const Item *item, bool binary_cmp) const
  {
    DBUG_ASSERT(is_basic_value(STRING_ITEM));
    return is_basic_value(item, STRING_ITEM) &&
           str_eq(value, ((Item_basic_value*)item)->val_str(NULL),
                  item->collation.collation, binary_cmp);
  }