  int save_in_field(Field *field, bool no_conversions)
  {
    field->set_notnull();
    return field->store_hex_hybrid(str_value.ptr(), str_value.length());
  }