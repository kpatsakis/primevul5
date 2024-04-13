  Item_float(THD *thd, double value_par, uint decimal_par):
    Item_num(thd), presentation(0), value(value_par)
  {
    decimals= (uint8) decimal_par;
    fixed= 1;
  }