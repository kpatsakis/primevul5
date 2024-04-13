  const Type_handler *real_type_handler() const
  {
    // Should not be called, Item_blob is used for SHOW purposes only.
    DBUG_ASSERT(0);
    return &type_handler_varchar;
  }