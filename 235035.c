  enum_field_types string_field_type() const
  {
    return Type_handler::string_type_handler(max_length)->field_type();
  }