big5_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end, OnigEncoding enc)
{
  const UChar *p;
  int len;

  if (s <= start) return (UChar* )s;
  p = s;

  if (BIG5_ISMB_TRAIL(*p)) {
    while (p > start) {
      if (! BIG5_ISMB_FIRST(*--p)) {
	p++;
	break;
      }
    }
  }
  len = enclen(enc, p, end);
  if (p + len > s) return (UChar* )p;
  p += len;
  return (UChar* )(p + ((s - p) & ~1));
}