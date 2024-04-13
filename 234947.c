  Item_string_ascii(THD *thd, const char *str, uint length):
    Item_string(thd, str, length, &my_charset_latin1,
                DERIVATION_COERCIBLE, MY_REPERTOIRE_ASCII)
  { }