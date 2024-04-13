  my_decimal *val_decimal(my_decimal *to)
  {
    return has_value() ? Time(this).to_decimal(to) : NULL;
  }