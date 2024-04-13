  Item_empty_string(THD *thd, const char *header,uint length,
                    CHARSET_INFO *cs= NULL):
    Item_partition_func_safe_string(thd, "", 0,
                                    cs ? cs : &my_charset_utf8_general_ci)
    { name=(char*) header; max_length= length * collation.collation->mbmaxlen; }