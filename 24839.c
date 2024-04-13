  Item_string(THD *thd, const String *str, CHARSET_INFO *tocs, uint *conv_errors,
              Derivation dv, uint repertoire)
   :Item_literal(thd)
  {
    if (str_value.copy(str, tocs, conv_errors))
      str_value.set("", 0, tocs); // EOM ?
    str_value.mark_as_const();
    fix_and_set_name_from_value(thd, dv, Metadata(&str_value, repertoire));
  }