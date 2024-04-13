  bool needs_charset_converter(CHARSET_INFO *tocs)
  {
    // Pass 1 as length to force conversion if tocs->mbminlen>1.
    return needs_charset_converter(1, tocs);
  }