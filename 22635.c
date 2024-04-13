static int to_ascii(OnigEncoding enc, UChar *s, UChar *end,
		    UChar buf[], int buf_size, int *is_over)
{
  int len;
  UChar *p;
  OnigCodePoint code;

  if (ONIGENC_MBC_MINLEN(enc) > 1) {
    p = s;
    len = 0;
    while (p < end) {
      code = ONIGENC_MBC_TO_CODE(enc, p, end);
      if (code >= 0x80) {
	if (code > 0xffff && len + 10 <= buf_size) {
	  sprint_byte_with_x((char*)(&(buf[len])), (unsigned int)(code >> 24));
	  sprint_byte((char*)(&(buf[len+4])),      (unsigned int)(code >> 16));
	  sprint_byte((char*)(&(buf[len+6])),      (unsigned int)(code >>  8));
	  sprint_byte((char*)(&(buf[len+8])),      (unsigned int)code);
	  len += 10;
	}
	else if (len + 6 <= buf_size) {
	  sprint_byte_with_x((char*)(&(buf[len])), (unsigned int)(code >> 8));
	  sprint_byte((char*)(&(buf[len+4])),      (unsigned int)code);
	  len += 6;
	}
	else {
	  break;
	}
      }
      else {
	buf[len++] = (UChar )code;
      }

      p += enclen(enc, p, end);
      if (len >= buf_size) break;
    }

    *is_over = ((p < end) ? 1 : 0);
  }
  else {
    len = (int)MIN((end - s), buf_size);
    xmemcpy(buf, s, (size_t )len);
    *is_over = ((buf_size < (end - s)) ? 1 : 0);
  }

  return len;
}