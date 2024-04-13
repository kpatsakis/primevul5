onigenc_ascii_get_case_fold_codes_by_str(OnigCaseFoldType flag ARG_UNUSED,
    const OnigUChar* p, const OnigUChar* end ARG_UNUSED, OnigCaseFoldCodeItem items[],
     OnigEncoding enc ARG_UNUSED)
{
  if (0x41 <= *p && *p <= 0x5a) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p + 0x20);
    return 1;
  }
  else if (0x61 <= *p && *p <= 0x7a) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p - 0x20);
    return 1;
  }
  else
    return 0;
}