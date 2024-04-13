  longlong val_int()
  {
    return m_value.to_datetime(current_thd).to_longlong();
  }