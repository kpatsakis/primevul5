mbc_to_code(const UChar* p, const UChar* end, OnigEncoding enc)
{
  int c, len;
  OnigCodePoint n;

  len = enclen(enc, p, end);
  c = *p++;
  if (len > 1) {
    len--;
    n = c & ((1 << (6 - len)) - 1);
    while (len--) {
      c = *p++;
      n = (n << 6) | (c & ((1 << 6) - 1));
    }
    return n;
  }
  else {
#ifdef USE_INVALID_CODE_SCHEME
    if (c > 0xfd) {
      return ((c == 0xfe) ? INVALID_CODE_FE : INVALID_CODE_FF);
    }
#endif
    return (OnigCodePoint )c;
  }
}