  Item_int(THD *thd, ulonglong i, size_t length= MY_INT64_NUM_DECIMAL_DIGITS):
    Item_num(thd), value((longlong)i)
    { max_length=(uint32)length; unsigned_flag= 1; }