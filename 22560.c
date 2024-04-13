euctw_is_allowed_reverse_match(const UChar* s, const UChar* end ARG_UNUSED, OnigEncoding enc ARG_UNUSED)
{
  const UChar c = *s;
  if (c <= 0x7e) return TRUE;
  else           return FALSE;
}