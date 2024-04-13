onigenc_utf16_32_get_ctype_code_range(OnigCtype ctype, OnigCodePoint* sb_out,
                                      const OnigCodePoint* ranges[],
				      struct OnigEncodingTypeST* enc ARG_UNUSED)
{
  *sb_out = 0x00;
  return onigenc_unicode_ctype_code_range(ctype, ranges);
}