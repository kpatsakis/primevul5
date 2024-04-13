  longlong val_int()
  {
    DBUG_ASSERT(sane());
    return null_value ? 0 :
           m_value.to_datetime(current_thd).to_longlong();
  }