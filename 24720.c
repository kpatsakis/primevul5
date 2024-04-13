  Item_int(THD *thd, longlong i,size_t length= MY_INT64_NUM_DECIMAL_DIGITS):
    Item_num(thd), value(i)
    { max_length=(uint32)length; }