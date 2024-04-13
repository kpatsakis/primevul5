  String *val_str(String *to)
  {
    DBUG_ASSERT(sane());
    return null_value ? NULL :
           m_value.to_datetime(current_thd).to_string(to, decimals);
  }