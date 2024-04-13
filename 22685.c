mbc_case_fold(OnigCaseFoldType flag, const UChar** pp,
		   const UChar* end, UChar* fold, OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (*p == 0x49) {
	*fold++ = 0xc4;
	*fold   = 0xb1;
	(*pp)++;
	return 2;
      }
    }
#endif

    *fold = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    (*pp)++;
    return 1; /* return byte length of converted char to lower */
  }
  else {
    return onigenc_unicode_mbc_case_fold(enc, flag, pp, end, fold);
  }
}