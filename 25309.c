  my_decimal *val_decimal(my_decimal *decimal_value)
  {
    DBUG_ASSERT(0); // never should be called
    null_value= true;
    return 0;
  }