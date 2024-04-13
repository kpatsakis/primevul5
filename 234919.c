  void fix_from_value(Derivation dv, const Metadata metadata)
  {
    fix_charset_and_length_from_str_value(dv, metadata);
    // it is constant => can be used without fix_fields (and frequently used)
    fixed= 1;
  }