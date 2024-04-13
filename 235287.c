  Item_partition_func_safe_string(THD *thd, const char *name_arg, uint length,
                                  CHARSET_INFO *cs= NULL):
    Item_string(thd, name_arg, length, cs)
  {}