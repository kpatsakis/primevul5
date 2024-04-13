  my_decimal *val_decimal(my_decimal *to)
  {
    return m_value.to_datetime(current_thd).to_decimal(to);
  }