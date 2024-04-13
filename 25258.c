  longlong val_time_packed(THD *thd)
  {
    if (check_null_ref())
      return 0;
    else
      return Item_direct_ref::val_time_packed(thd);
  }