  my_decimal *val_decimal(my_decimal *tmp)
  {
    if (check_null_ref())
      return NULL;
    else
      return Item_direct_ref::val_decimal(tmp);
  }