get_ctype_code_range(OnigCtype ctype, OnigCodePoint *sb_out,
			  const OnigCodePoint* ranges[], OnigEncoding enc ARG_UNUSED)
{
  *sb_out = 0x80;
  return onigenc_unicode_ctype_code_range(ctype, ranges);
}