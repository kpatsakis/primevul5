  my_decimal *val_decimal(my_decimal *to)
  {
    return has_value() ? Date(this).to_decimal(to) : NULL;
  }