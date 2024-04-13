  my_decimal *val_decimal(my_decimal *decimal_value)
  {
    // following assert is redundant, because fixed=1 assigned in constructor
    DBUG_ASSERT(fixed == 1);
    ulonglong value= (ulonglong) Item_hex_hybrid::val_int();
    int2my_decimal(E_DEC_FATAL_ERROR, value, TRUE, decimal_value);
    return decimal_value;
  }