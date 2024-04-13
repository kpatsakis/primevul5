  virtual bool check_vcol_func_processor(void *arg)
  {
    return mark_unsupported_function(full_name(), arg, VCOL_IMPOSSIBLE);
  }