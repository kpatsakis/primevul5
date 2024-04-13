  Item_string_ascii(THD *thd, const char *str):
    Item_string(thd, str, (uint)strlen(str), &my_charset_latin1,
                DERIVATION_COERCIBLE, MY_REPERTOIRE_ASCII)
  { }