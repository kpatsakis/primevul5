gbk_mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc ARG_UNUSED)
{
  int firstbyte = *p++;
  state_t s = trans[0][firstbyte];
#define RETURN(n) \
    return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(n) : \
                         ONIGENC_CONSTRUCT_MBCLEN_INVALID()
  if (s < 0) RETURN(1);
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(EncLen_GBK[firstbyte]-1);
  s = trans[s][*p++];
  RETURN(2);
#undef RETURN
}