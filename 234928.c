  Item_hex_constant(THD *thd): Item_basic_constant(thd)
  {
    hex_string_init(thd, "", 0);
  }