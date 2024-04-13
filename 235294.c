  Item_string(THD *thd, const char *name_par, const char *str, uint length,
              CHARSET_INFO *cs, Derivation dv= DERIVATION_COERCIBLE):
    Item_basic_constant(thd)
  {
    str_value.set_or_copy_aligned(str, length, cs);
    fix_from_value(dv, Metadata(&str_value));
    set_name(thd, name_par, 0, system_charset_info);
  }