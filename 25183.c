  Item_string(THD *thd, const char *str, size_t length,
              CHARSET_INFO *cs, Derivation dv= DERIVATION_COERCIBLE)
   :Item_literal(thd)
  {
    str_value.set_or_copy_aligned(str, length, cs);
    fix_and_set_name_from_value(thd, dv, Metadata(&str_value));
  }