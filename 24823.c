  my_decimal *val_decimal(my_decimal *dec)
  {
    return can_return_value() ? value.val_decimal(dec, this) : NULL;
  }