  Item_static_string_func(THD *thd, const char *name_par,
                          const String *str,
                          CHARSET_INFO *tocs, uint *conv_errors,
                          Derivation dv, uint repertoire):
    Item_string(thd, str, tocs, conv_errors, dv, repertoire),
    func_name(name_par)
  {}