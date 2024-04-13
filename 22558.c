is_allowed_reverse_match(const UChar* s, const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  const UChar c = *s;
  return (SJIS_ISMB_TRAIL(c) ? FALSE : TRUE);
}