  Item_hex_constant(THD *thd, const char *str, size_t str_length):
    Item_literal(thd)
  {
    hex_string_init(thd, str, str_length);
  }