  virtual Item *safe_charset_converter(THD *thd, CHARSET_INFO *tocs)
  {
    return const_charset_converter(thd, tocs, true);
  }