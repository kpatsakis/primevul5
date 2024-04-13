  bool fix_fields(THD *thd, Item **it)
  {
    if ((!(*ref)->fixed && (*ref)->fix_fields(thd, ref)) ||
        (*ref)->check_cols(1))
      return TRUE;
    return Item_ref::fix_fields(thd, it);
  }