  bool has_int_value() const
  {
    return state == SHORT_DATA_VALUE &&
           value.type_handler()->cmp_type() == INT_RESULT;
  }