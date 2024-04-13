  Item_int(THD *thd, const char *str_arg,longlong i,size_t length):
    Item_num(thd), value(i)
    {
      max_length=(uint32)length;
      name.str= str_arg; name.length= safe_strlen(name.str);
    }