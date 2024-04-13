onigenc_mbclen_approximate(const OnigUChar* p,const OnigUChar* e, struct OnigEncodingTypeST* enc)
{
  int ret = ONIGENC_PRECISE_MBC_ENC_LEN(enc,p,e);
  if (ONIGENC_MBCLEN_CHARFOUND_P(ret))
    return ONIGENC_MBCLEN_CHARFOUND_LEN(ret);
  else if (ONIGENC_MBCLEN_NEEDMORE_P(ret))
    return (int)(e-p)+ONIGENC_MBCLEN_NEEDMORE_LEN(ret);
  return 1;
}