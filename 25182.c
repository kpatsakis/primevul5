  longlong val_int()
  {
    return update_null() ? 0 : cached_time.to_longlong();
  }