  int save_in_field(Field *field, bool no_conversions)
  {
    DBUG_ASSERT(sane());
    if (null_value)
      return set_field_to_null(field);
    Timestamp_or_zero_datetime_native native(m_value, decimals);
    return native.save_in_field(field, decimals);
  }