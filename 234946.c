  bool str_eq(const String *value,
              const String *other, CHARSET_INFO *cs, bool binary_cmp) const
  {
    return binary_cmp ?
      value->bin_eq(other) :
      collation.collation == cs && value->eq(other, collation.collation);
  }