utf16le_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end,
			      OnigEncoding enc ARG_UNUSED)
{
  if (s <= start) return (UChar* )s;

  if ((s - start) % 2 == 1) {
    s--;
  }

  if (UTF16_IS_SURROGATE_SECOND(*(s+1)) && s > start + 1)
    s -= 2;

  return (UChar* )s;
}