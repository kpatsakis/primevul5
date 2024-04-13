  void copy()
  {
    Timestamp_or_zero_datetime_native_null tmp(current_thd, item, false);
    null_value= tmp.is_null();
    m_value= tmp.is_null() ? Timestamp_or_zero_datetime() :
                             Timestamp_or_zero_datetime(tmp);
  }