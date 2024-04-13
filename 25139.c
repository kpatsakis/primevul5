  bool val_native(THD *thd, Native *to)
  {
    if (check_null_ref())
      return true;
    return Item_direct_ref::val_native(thd, to);
  }