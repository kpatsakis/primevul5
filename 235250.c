  Item *const_charset_converter(THD *thd, CHARSET_INFO *tocs, bool lossless)
  { return const_charset_converter(thd, tocs, lossless, NULL); }