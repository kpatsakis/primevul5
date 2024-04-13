  void fix_and_set_name_from_value(THD *thd, Derivation dv,
                                   const Metadata metadata)
  {
    fix_from_value(dv, metadata);
    set_name(thd, str_value.ptr(), str_value.length(), str_value.charset());
  }