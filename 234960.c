  bool check_vcol_func_processor(void *arg) 
  {
    return mark_unsupported_function("name_const()", arg, VCOL_IMPOSSIBLE);
  }