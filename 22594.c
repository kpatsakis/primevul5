utf32be_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end,
			      OnigEncoding enc ARG_UNUSED)
{
  ptrdiff_t rem;

  if (s <= start) return (UChar* )s;

  rem = (s - start) % 4;
  return (UChar* )(s - rem);
}