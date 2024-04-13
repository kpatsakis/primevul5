onigenc_mbn_is_mbc_ambiguous(OnigEncoding enc, OnigCaseFoldType flag,
                             const UChar** pp ARG_UNUSED, const UChar* end ARG_UNUSED)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    (*pp)++;
    return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);
  }

  (*pp) += enclen(enc, p);
  return FALSE;
}