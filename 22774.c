code_to_mbc(OnigCodePoint code, UChar *buf, OnigEncoding enc)
{
  UChar *p = buf;

  if ((code & 0xff000000) != 0) *p++ = (UChar )(((code >> 24) & 0xff));
  if ((code &   0xff0000) != 0) *p++ = (UChar )(((code >> 16) & 0xff));
  if ((code &     0xff00) != 0) *p++ = (UChar )(((code >>  8) & 0xff));
  *p++ = (UChar )(code & 0xff);

  if (enclen(enc, buf, p) != (p - buf))
    return ONIGERR_INVALID_CODE_POINT_VALUE;
  return (int)(p - buf);
}