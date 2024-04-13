  void fix_charset_and_length_from_str_value(const String &str, Derivation dv)
  {
    fix_charset_and_length(str.charset(), dv, Metadata(&str));
  }