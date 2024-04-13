  bool fix_fields_if_needed_for_order_by(THD *thd, Item **ref)
  {
    return fix_fields_if_needed_for_scalar(thd, ref);
  }