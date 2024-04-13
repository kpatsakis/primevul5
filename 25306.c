  const Type_handler *type_handler() const
  {
    return Type_handler_hybrid_field_type::type_handler()->
             type_handler_for_item_field();
  }