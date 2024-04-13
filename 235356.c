  String *val_str(String* tmp)
  {
    if (check_null_ref())
      return NULL;
    else
      return Item_direct_ref::val_str(tmp);
  }