  const char *field_name_or_null()
  { return real_item()->type() == Item::FIELD_ITEM ? name : NULL; }