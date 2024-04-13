  bool fix_fields_if_needed_for_scalar(THD *thd, Item **ref)
  {
    return fix_fields_if_needed(thd, ref) || check_cols(1);
  }