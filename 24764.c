  bool check_vcol_func_processor(void *arg)
  {
    return mark_unsupported_function("value()", arg, VCOL_IMPOSSIBLE);
  }