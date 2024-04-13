  Item_hex_constant(THD *thd, const char *str, uint str_length):
    Item_basic_constant(thd)
  {
    hex_string_init(thd, str, str_length);
  }