  Item_int(THD *thd, ulonglong i, uint length= MY_INT64_NUM_DECIMAL_DIGITS):
    Item_num(thd), value((longlong)i)
    { max_length=length; fixed= 1; unsigned_flag= 1; }