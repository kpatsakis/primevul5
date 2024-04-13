  void save_val(Field *to)
  {
    if (check_null_ref())
      to->set_null();
    else
      Item_direct_ref::save_val(to);
  }