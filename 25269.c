  String *val_str(String *to)
  {
    return has_value() ? Time(this).to_string(to, decimals) : NULL;
  }