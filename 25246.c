  const Type_handler *real_type_handler() const
  {
    if (field->is_created_from_null_item)
      return &type_handler_null;
    return field->type_handler();
  }