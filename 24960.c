  const Type_handler *type_handler() const
  {
    if (result_field)
      return result_field->type_handler();
    return &type_handler_null;
  }