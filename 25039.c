  longlong val_datetime_packed(THD *thd)
  {
    return to_datetime(current_thd).to_packed();
  }