  String *val_str(String *to)
  {
    return update_null() ? 0 : cached_time.to_string(to);
  }