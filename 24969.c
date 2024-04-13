  bool set_limit_clause_param(longlong nr)
  {
    value.set_handler(&type_handler_longlong);
    set_int(nr, MY_INT64_NUM_DECIMAL_DIGITS);
    return !unsigned_flag && value.integer < 0;
  }