  String *val_str(String *to)
  {
    return has_value() ? Datetime(this).to_string(to, decimals) : NULL;
  }