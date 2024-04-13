  my_decimal *val_decimal(my_decimal *decimal_value)
  {
    longlong value= Item_hex_hybrid::val_int();
    int2my_decimal(E_DEC_FATAL_ERROR, value, TRUE, decimal_value);
    return decimal_value;
  }