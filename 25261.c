  my_decimal *val_decimal(my_decimal *to)
  {
    DBUG_ASSERT(sane());
    return null_value ? NULL :
           m_value.to_datetime(current_thd).to_decimal(to);
  }