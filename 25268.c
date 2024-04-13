  my_decimal *val_decimal(my_decimal *to)
  {
    return update_null() ? 0 : cached_time.to_decimal(to);
  }