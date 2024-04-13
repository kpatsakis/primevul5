utf16le_mbc_enc_len(const UChar* p, const OnigUChar* e,
		    OnigEncoding enc ARG_UNUSED)
{
  int len = (int)(e - p);
  UChar byte;
  if (len < 2)
    return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
  byte = p[1];
  if (!UTF16_IS_SURROGATE(byte)) {
    return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(2);
  }
  if (UTF16_IS_SURROGATE_FIRST(byte)) {
    if (len < 4)
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(4-len);
    if (UTF16_IS_SURROGATE_SECOND(p[3]))
      return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(4);
  }
  return ONIGENC_CONSTRUCT_MBCLEN_INVALID();
}