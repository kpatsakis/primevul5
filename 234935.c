  bool is_basic_value(Type type_arg) const
  {
    return basic_const_item() && type() == type_arg;
  }