  bool val_bool()
  {
    if (check_null_ref())
      return 0;
    else
      return Item_direct_ref::val_bool();
  }