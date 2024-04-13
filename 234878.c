  Item_int(THD *thd, longlong i,uint length= MY_INT64_NUM_DECIMAL_DIGITS):
    Item_num(thd), value(i)
    { max_length=length; fixed= 1; }