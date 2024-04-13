onigenc_mb2_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff00) != 0) {
    *p++ = (UChar )((code >>  8) & 0xff);
  }
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enclen(enc, buf, p) != (p - buf))
    return ONIGERR_INVALID_CODE_POINT_VALUE;
#endif
  return (int)(p - buf);
}