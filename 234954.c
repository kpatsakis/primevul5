  Item_string_with_introducer(THD *thd, const char *str, uint length,
                              CHARSET_INFO *cs):
    Item_string(thd, str, length, cs)
  { }