utf32be_mbc_case_fold(OnigCaseFoldType flag,
		      const UChar** pp, const UChar* end, UChar* fold,
		      OnigEncoding enc)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_ASCII_CODE(*(p+3)) && *(p+2) == 0 && *(p+1) == 0 && *p == 0) {
    *fold++ = 0;
    *fold++ = 0;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (*(p+3) == 0x49) {
	*fold++ = 0x01;
	*fold   = 0x31;
	(*pp) += 4;
	return 4;
      }
    }
#endif

    *fold++ = 0;
    *fold   = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*(p+3));
    *pp += 4;
    return 4;
  }
  else
    return onigenc_unicode_mbc_case_fold(enc, flag, pp,
					 end, fold);
}