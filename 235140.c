  Item_string_sys(THD *thd, const char *str, uint length):
    Item_string(thd, str, length, system_charset_info)
  { }