  int save_in_field(Field *field, bool no_conversions)
  {
    Timestamp_or_zero_datetime_native native(m_value, decimals);
    return native.save_in_field(field, decimals);
  }