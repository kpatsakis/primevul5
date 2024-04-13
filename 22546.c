mbc_case_fold(OnigCaseFoldType flag,
	      const UChar** pp, const UChar* end, UChar* lower,
	      OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    (*pp)++;
    return 1;
  }
  else {
    int i;
    int len = enclen(enc, p, end);

    for (i = 0; i < len; i++) {
      *lower++ = *p++;
    }
    (*pp) += len;
    return len; /* return byte length of converted char to lower */
  }
}