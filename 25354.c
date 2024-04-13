  bool fix_fields_if_needed_for_bool(THD *thd, Item **ref)
  {
    return fix_fields_if_needed_for_scalar(thd, ref);
  }