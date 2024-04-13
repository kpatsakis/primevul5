  Item_static_float_func(THD *thd, const char *str, double val_arg,
                         uint decimal_par, uint length):
    Item_float(thd, NullS, val_arg, decimal_par, length), func_name(str)
  {}