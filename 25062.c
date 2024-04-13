  const Type_handler *type_handler() const
  {
    return Type_handler::blob_type_handler(max_length);
  }