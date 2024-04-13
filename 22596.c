koi8_r_is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp,
			const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  int v;
  const UChar* p = *pp;

  (*pp)++;
  v = (EncKOI8_R_CtypeTable[*p] & (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
  return (v != 0 ? TRUE : FALSE);
}