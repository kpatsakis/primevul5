is_mbc_newline(const UChar* p, const UChar* end, OnigEncoding enc)
{
  if (p < end) {
    if (*p == 0x0a) return 1;

#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
#ifndef USE_CRNL_AS_LINE_TERMINATOR
    if (*p == 0x0d) return 1;
#endif
    if (p + 1 < end) {
      if (*(p+1) == 0x85 && *p == 0xc2) /* U+0085 */
	return 1;
      if (p + 2 < end) {
	if ((*(p+2) == 0xa8 || *(p+2) == 0xa9)
	    && *(p+1) == 0x80 && *p == 0xe2)  /* U+2028, U+2029 */
	  return 1;
      }
    }
#endif
  }

  return 0;
}