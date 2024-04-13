  bool cleanup_excluding_fields_processor(void *arg)
  { return field ? 0 : cleanup_processor(arg); }