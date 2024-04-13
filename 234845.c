  bool eq(const Item *item, bool binary_cmp) const
  {
    return item->basic_const_item() && item->type() == type() &&
           item->cast_to_int_type() == cast_to_int_type() &&
           str_value.bin_eq(&((Item_hex_constant*)item)->str_value);
  }