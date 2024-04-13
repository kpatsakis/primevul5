  CHARSET_INFO *charset_for_protocol(void) const
  {
    return type_handler()->charset_for_protocol(this);
  };