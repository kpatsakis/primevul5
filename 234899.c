  bool cleanup_excluding_const_fields_processor(void *arg)
  { return field && const_item() ? 0 : cleanup_processor(arg); }