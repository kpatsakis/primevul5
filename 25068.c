  void fix_from_value(Derivation dv, const Metadata metadata)
  {
    fix_charset_and_length(str_value.charset(), dv, metadata);
  }