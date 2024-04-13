  my_decimal *val_decimal(my_decimal *to)
  {
    return to_datetime(current_thd).to_decimal(to);
  }