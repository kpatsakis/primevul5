koi8_r_mbc_case_fold(OnigCaseFoldType flag ARG_UNUSED,
		     const UChar** pp, const UChar* end ARG_UNUSED,
		     UChar* lower, OnigEncoding enc ARG_UNUSED)
{
  const UChar* p = *pp;

  *lower = ENC_KOI8_R_TO_LOWER_CASE(*p);
  (*pp)++;
  return 1;
}