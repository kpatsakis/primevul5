  int save_in_field(Field *field, bool no_conversions)
  {
    field->set_notnull();
    return field->store(str_value.ptr(), str_value.length(), 
                        collation.collation);
  }