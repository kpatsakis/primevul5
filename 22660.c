utf32be_code_to_mbc(OnigCodePoint code, UChar *buf,
		    OnigEncoding enc ARG_UNUSED)
{
  UChar* p = buf;

  *p++ = (UChar )((code & 0xff000000) >>24);
  *p++ = (UChar )((code & 0xff0000)   >>16);
  *p++ = (UChar )((code & 0xff00)     >> 8);
  *p++ = (UChar ) (code & 0xff);
  return 4;
}