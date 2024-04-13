mbc_case_fold(OnigCaseFoldType flag,
			  const UChar** pp, const UChar* end ARG_UNUSED,
			  UChar* lower, OnigEncoding enc ARG_UNUSED)
{
  const UChar* p = *pp;

  if (*p == 0xdf && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    *lower++ = 's';
    *lower   = 's';
    (*pp)++;
    return 2;
  }

  *lower = ENC_ISO_8859_15_TO_LOWER_CASE(*p);
  (*pp)++;
  return 1; /* return byte length of converted char to lower */
}