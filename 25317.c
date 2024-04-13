  Item_string(THD *thd, const char *name_par, const char *str, size_t length,
              CHARSET_INFO *cs, Derivation dv= DERIVATION_COERCIBLE)
   :Item_literal(thd)
  {
    str_value.set_or_copy_aligned(str, length, cs);
    fix_from_value(dv, Metadata(&str_value));
    set_name(thd, name_par,safe_strlen(name_par), system_charset_info);
  }