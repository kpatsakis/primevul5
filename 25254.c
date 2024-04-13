  String *val_str(String *to)
  {
    return has_value() ? Date(this).to_string(to) : NULL;
  }