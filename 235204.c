  bool eq(const Item *item, bool binary_cmp) const
  {
    return str_eq(&str_value, item, binary_cmp);
  }