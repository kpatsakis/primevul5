  bool can_return_const_value(Item_result type) const
  {
    return can_return_value() &&
           value.type_handler()->cmp_type() == type &&
           type_handler()->cmp_type() == type;
  }