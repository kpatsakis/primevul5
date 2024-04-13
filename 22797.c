gbk_is_allowed_reverse_match(const UChar* s, const UChar* end ARG_UNUSED, OnigEncoding enc ARG_UNUSED)
{
  const UChar c = *s;
  return (GBK_ISMB_TRAIL(c) ? FALSE : TRUE);
}