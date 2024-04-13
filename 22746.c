gb18030_mbc_to_code(const UChar* p, const UChar* end, OnigEncoding enc)
{
  int c, i, len;
  OnigCodePoint n;

  len = enclen(enc, p, end);
  n = (OnigCodePoint )(*p++);
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}