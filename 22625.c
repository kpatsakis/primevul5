mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc ARG_UNUSED)
{
  int firstbyte = *p++;
  state_t s;
  s = trans[0][firstbyte];
  if (s < 0) return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(1) :
                                  ONIGENC_CONSTRUCT_MBCLEN_INVALID();
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(EncLen_SJIS[firstbyte]-1);
  s = trans[s][*p++];
  return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(2) :
                       ONIGENC_CONSTRUCT_MBCLEN_INVALID();
}