  int save_in_field(Field *field_arg, bool no_conversions)
  {
    return field_arg->save_in_field_ignore_value(false);
  }