  Item_string(THD *thd, CHARSET_INFO *cs, Derivation dv= DERIVATION_COERCIBLE):
    Item_basic_constant(thd)
  {
    collation.set(cs, dv);
    max_length= 0;
    set_name(thd, NULL, 0, system_charset_info);
    decimals= NOT_FIXED_DEC;
    fixed= 1;
  }