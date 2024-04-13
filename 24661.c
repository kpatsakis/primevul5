  String *val_str(String *str)
  {
    return can_return_value() ? value.val_str(str, this) : NULL;
  }