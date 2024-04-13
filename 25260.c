  bool fix_fields_if_needed(THD *thd, Item **ref)
  {
    return is_fixed() ? false : fix_fields(thd, ref);
  }