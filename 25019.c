  const Type_handler *type_handler() const
  {
    return Type_handler::get_handler_by_field_type(int_field_type);
  }