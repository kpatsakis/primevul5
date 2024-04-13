onigenc_ascii_mbc_case_fold(OnigCaseFoldType flag ARG_UNUSED, const UChar** p,
			    const UChar*end, UChar* lower, OnigEncoding enc ARG_UNUSED)
{
  *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(**p);

  (*p)++;
  return 1; /* return byte length of converted char to lower */
}