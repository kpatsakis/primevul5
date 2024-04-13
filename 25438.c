  Item_hex_constant(THD *thd): Item_literal(thd)
  {
    hex_string_init(thd, "", 0);
  }