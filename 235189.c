  Item_int(THD *thd, const char *str_arg,longlong i,uint length):
    Item_num(thd), value(i)
    { max_length=length; name=(char*) str_arg; fixed= 1; }