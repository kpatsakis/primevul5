  bool is_basic_value(const Item *item, Type type_arg) const
  {
    return item->basic_const_item() && item->type() == type_arg;
  }