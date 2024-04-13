  longlong val_datetime_packed(THD *thd)
  {
    return update_null() ? 0 : cached_time.valid_datetime_to_packed();
  }