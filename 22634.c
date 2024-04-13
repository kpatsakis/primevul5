onigenc_get_right_adjust_char_head(OnigEncoding enc, const UChar* start, const UChar* s, const UChar* end)
{
  UChar* p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s, end);
  if (p < s) {
      p += enclen(enc, p, end);
  }
  return p;
}