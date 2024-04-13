big5_is_mbc_ambiguous(OnigCaseFoldType flag,
		      const UChar** pp, const UChar* end, OnigEncoding enc)
{
  return onigenc_mbn_is_mbc_ambiguous(enc, flag, pp, end);
}