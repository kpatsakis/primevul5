  Item_string_sys(THD *thd, const char *str):
    Item_string(thd, str, (uint) strlen(str), system_charset_info)
  { }