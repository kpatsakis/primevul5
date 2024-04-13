  Item_string(THD *thd, CHARSET_INFO *csi, const char *str_arg, uint length_arg):
    Item_basic_constant(thd)
  {
    collation.set(csi, DERIVATION_COERCIBLE);
    set_name(thd, NULL, 0, system_charset_info);
    decimals= NOT_FIXED_DEC;
    fixed= 1;
    str_value.copy(str_arg, length_arg, csi);
    max_length= str_value.numchars() * csi->mbmaxlen;
  }