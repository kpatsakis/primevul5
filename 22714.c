left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  const UChar *p;

  if (s <= start) return (UChar* )s;
  p = s;

  while (!utf8_islead(*p) && p > start) p--;
  return (UChar* )p;
}