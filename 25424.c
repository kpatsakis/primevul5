  String *val_str(String *to)
  {
    return m_value.to_datetime(current_thd).to_string(to, decimals);
  }