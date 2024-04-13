  Item_string(THD *thd, const char *str, uint length, CHARSET_INFO *cs,
              Derivation dv, uint repertoire)
   :Item_literal(thd)
  {
    str_value.set_or_copy_aligned(str, length, cs);
    fix_and_set_name_from_value(thd, dv, Metadata(&str_value, repertoire));
  }