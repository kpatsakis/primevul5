  Item_float(THD *thd, const char *str, double val_arg, uint decimal_par,
             uint length): Item_num(thd), value(val_arg)
  {
    presentation= name.str= str;
    name.length= safe_strlen(str);
    decimals=(uint8) decimal_par;
    max_length= length;
  }