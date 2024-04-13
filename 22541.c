gb18030_is_allowed_reverse_match(const UChar* s, const UChar* end ARG_UNUSED, OnigEncoding enc ARG_UNUSED)
{
  return GB18030_MAP[*s] == C1 ? TRUE : FALSE;
}