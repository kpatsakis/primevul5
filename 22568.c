is_allowed_reverse_match(const UChar* s, const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  const UChar c = *s;
  if (c <= 0x7e || c == 0x8e || c == 0x8f)
    return TRUE;
  else
    return FALSE;
}