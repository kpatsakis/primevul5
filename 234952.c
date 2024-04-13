  Item_float(THD *thd, const char *str, double val_arg, uint decimal_par,
             uint length): Item_num(thd), value(val_arg)
  {
    presentation= name=(char*) str;
    decimals=(uint8) decimal_par;
    max_length=length;
    fixed= 1;
  }