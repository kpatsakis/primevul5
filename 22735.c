big5_mbc_enc_len0(const UChar* p, const UChar* e, int tridx, const int tbl[])
{
  int firstbyte = *p++;
  state_t s = trans[tridx][firstbyte];
#define RETURN(n) \
    return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(n) : \
                         ONIGENC_CONSTRUCT_MBCLEN_INVALID()
  if (s < 0) RETURN(1);
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(tbl[firstbyte]-1);
  s = trans[s][*p++];
  RETURN(2);
#undef RETURN
}