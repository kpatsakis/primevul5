utf16le_mbc_to_code(const UChar* p, const UChar* end ARG_UNUSED,
		    OnigEncoding enc ARG_UNUSED)
{
  OnigCodePoint code;
  UChar c0 = *p;
  UChar c1 = *(p+1);

  if (UTF16_IS_SURROGATE_FIRST(c1)) {
    code = ((((c1 << 8) + c0) & 0x03ff) << 10)
         + (((p[3] << 8) + p[2]) & 0x03ff) + 0x10000;
  }
  else {
    code = c1 * 256 + p[0];
  }
  return code;
}