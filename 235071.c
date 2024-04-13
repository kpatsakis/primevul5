  virtual CHARSET_INFO *charset_for_protocol(void) const
  {
    return cmp_type() == STRING_RESULT ? collation.collation :
                                         &my_charset_bin;
  };