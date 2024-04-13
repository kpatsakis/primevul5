  Item_blob(THD *thd, const char *name_arg, uint length):
    Item_partition_func_safe_string(thd, name_arg, (uint) strlen(name_arg), &my_charset_bin)
  { max_length= length; }