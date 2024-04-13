code_to_mbc(OnigCodePoint code, UChar *buf, OnigEncoding enc)
{
  UChar *p = buf;

  if ((code & 0xff00) != 0) *p++ = (UChar )(((code >>  8) & 0xff));
  *p++ = (UChar )(code & 0xff);

#if 0
  if (enclen(enc, buf) != (p - buf))
    return REGERR_INVALID_CODE_POINT_VALUE;
#endif
  return (int)(p - buf);
}