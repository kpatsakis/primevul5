  bool is_null()
  {
    if (check_null_ref())
      return 1;
    else
      return Item_direct_ref::is_null();
  }