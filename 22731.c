property_name_to_ctype(OnigEncoding enc, UChar* p, UChar* end)
{
  st_data_t ctype;
  UChar *s, *e;

  PROPERTY_LIST_INIT_CHECK;

  s = e = ALLOCA_N(UChar, end-p+1);
  for (; p < end; p++) {
    *e++ = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
  }

  if (onig_st_lookup_strend(PropertyNameTable, s, e, &ctype) == 0) {
    return onigenc_minimum_property_name_to_ctype(enc, s, e);
  }

  return (int)ctype;
}