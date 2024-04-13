  bool fix_fields(THD *thd, Item **it)
  {
    if ((*ref)->fix_fields_if_needed_for_scalar(thd, ref))
      return TRUE;
    return Item_ref::fix_fields(thd, it);
  }