  Item_string(THD *thd, const char *name_par, const char *str, size_t length,
              CHARSET_INFO *cs, Derivation dv, uint repertoire)
   :Item_literal(thd)
  {
    str_value.set_or_copy_aligned(str, length, cs);
    fix_from_value(dv, Metadata(&str_value, repertoire));
    set_name(thd, name_par, safe_strlen(name_par), system_charset_info);
  }