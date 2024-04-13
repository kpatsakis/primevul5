  Item_bool(THD *thd, const char *str_arg, longlong i):
    Item_int(thd, str_arg, i, 1) {}