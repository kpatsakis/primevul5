  Item_int(THD *thd, int32 i,size_t length= MY_INT32_NUM_DECIMAL_DIGITS):
    Item_num(thd), value((longlong) i)
    { max_length=(uint32)length; }