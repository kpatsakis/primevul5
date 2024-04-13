  Item_return_date_time(THD *thd, const char *name_arg, uint length_arg,
                        enum_field_types field_type_arg):
    Item_partition_func_safe_string(thd, name_arg, length_arg, &my_charset_bin),
    date_time_field_type(field_type_arg)
  { decimals= 0; }