utf16be_code_to_mbc(OnigCodePoint code, UChar *buf,
		    OnigEncoding enc)
{
  UChar* p = buf;

  if (code > 0xffff) {
    unsigned int high = (code >> 10) + 0xD7C0;
    unsigned int low = (code & 0x3FF) + 0xDC00;
    *p++ = (high >> 8) & 0xFF;
    *p++ = high & 0xFF;
    *p++ = (low >> 8) & 0xFF;
    *p++ = low & 0xFF;
    return 4;
  }
  else {
    *p++ = (UChar )((code & 0xff00) >> 8);
    *p++ = (UChar )(code & 0xff);
    return 2;
  }
}