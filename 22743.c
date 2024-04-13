utf16le_mbc_case_fold(OnigCaseFoldType flag,
		      const UChar** pp, const UChar* end, UChar* fold,
		      OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_ASCII_CODE(*p) && *(p+1) == 0) {
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (*p == 0x49) {
	*fold++ = 0x31;
	*fold   = 0x01;
	(*pp) += 2;
	return 2;
      }
    }
#endif

    *fold++ = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    *fold   = 0;
    *pp += 2;
    return 2;
  }
  else
    return onigenc_unicode_mbc_case_fold(enc, flag, pp,
					 end, fold);
}