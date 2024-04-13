  Item_string_with_introducer(THD *thd, const char *name_arg,
                              const char *str, uint length, CHARSET_INFO *tocs):
    Item_string(thd, name_arg, str, length, tocs)
  { }