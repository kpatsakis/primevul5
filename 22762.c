left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end, OnigEncoding enc)
{
  const UChar *p;

  if (s <= start) return (UChar* )s;
  p = s;

  while (!emacsmule_islead(*p) && p > start) p--;
  return (UChar* )p;
}