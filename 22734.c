utf16be_mbc_to_code(const UChar* p, const UChar* end ARG_UNUSED,
		    OnigEncoding enc)
{
  OnigCodePoint code;

  if (UTF16_IS_SURROGATE_FIRST(*p)) {
    code = ((((p[0] << 8) + p[1]) & 0x03ff) << 10)
         + (((p[2] << 8) + p[3]) & 0x03ff) + 0x10000;
  }
  else {
    code = p[0] * 256 + p[1];
  }
  return code;
}