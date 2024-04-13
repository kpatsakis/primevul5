  uchar* global_value_ptr(THD *thd, const LEX_STRING *base)
  { return do_value_ptr(thd, OPT_GLOBAL, base); }