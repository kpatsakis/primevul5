  String* val_str(String *to)
  {
    return to_datetime(current_thd).to_string(to, decimals);
  }