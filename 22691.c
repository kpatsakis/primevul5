onig_vsnprintf_with_pattern(UChar buf[], int bufsize, OnigEncoding enc,
                           UChar* pat, UChar* pat_end, const UChar *fmt, va_list args)
{
  size_t need;
  int n, len;
  UChar *p, *s, *bp;
  UChar bs[6];

  n = xvsnprintf((char* )buf, bufsize, (const char* )fmt, args);

  need = (pat_end - pat) * 4 + 4;

  if (n + need < (size_t)bufsize) {
    strcat((char* )buf, ": /");
    s = buf + onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, buf);

    p = pat;
    while (p < pat_end) {
      if (*p == '\\') {
	*s++ = *p++;
	len = enclen(enc, p, pat_end);
	while (len-- > 0) *s++ = *p++;
      }
      else if (*p == '/') {
	*s++ = (unsigned char )'\\';
	*s++ = *p++;
      }
      else if (ONIGENC_IS_MBC_HEAD(enc, p, pat_end)) {
        len = enclen(enc, p, pat_end);
        if (ONIGENC_MBC_MINLEN(enc) == 1) {
          while (len-- > 0) *s++ = *p++;
        }
        else { /* for UTF16 */
          int blen;

          while (len-- > 0) {
            sprint_byte_with_x((char* )bs, (unsigned int )(*p++));
            blen = onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, bs);
            bp = bs;
            while (blen-- > 0) *s++ = *bp++;
          }
        }
      }
      else if (!ONIGENC_IS_CODE_PRINT(enc, *p) &&
	       !ONIGENC_IS_CODE_SPACE(enc, *p)) {
	sprint_byte_with_x((char* )bs, (unsigned int )(*p++));
	len = onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, bs);
        bp = bs;
	while (len-- > 0) *s++ = *bp++;
      }
      else {
	*s++ = *p++;
      }
    }

    *s++ = '/';
    *s   = '\0';
  }
}