  String *val_str(String *to)
  {
    return update_null() ? NULL : cached_time.to_string(to, decimals);
  }