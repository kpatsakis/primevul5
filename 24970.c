  bool is_of_type(Type t, Item_result cmp) const
  {
    return type() == t && cmp_type() == cmp;
  }