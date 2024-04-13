  bool check_vcol_func_processor(void *arg)
  { // VCOL_TIME_FUNC because the value is not constant, but does not
    // require fix_fields() to be re-run for every statement.
    return mark_unsupported_function(func_name, arg, VCOL_TIME_FUNC);
  }