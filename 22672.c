euctw_mbc_case_fold(OnigCaseFoldType flag, const UChar** pp, const UChar* end,
                    UChar* lower, OnigEncoding enc)
{
  return onigenc_mbn_mbc_case_fold(enc, flag,
                                   pp, end, lower);
}