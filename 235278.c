  bool check_vcol_func_processor(void *arg)
  {
    return mark_unsupported_function("values()", arg, VCOL_IMPOSSIBLE);
  }