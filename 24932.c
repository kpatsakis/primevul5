  const Type_handler *type_handler() const
  {
    const Type_handler *handler= field->type_handler();
    return handler->type_handler_for_item_field();
  }