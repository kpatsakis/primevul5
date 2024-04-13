rb_str_inspect(VALUE str)
{
    rb_encoding *enc = STR_ENC_GET(str);
    const char *p, *pend, *prev;
#define CHAR_ESC_LEN 12 /* sizeof(\x{ hex of 32bit unsigned int }) */
    char buf[CHAR_ESC_LEN + 1];
    VALUE result = rb_str_buf_new(0);
    rb_encoding *resenc = rb_default_internal_encoding();
    int unicode_p = rb_enc_unicode_p(enc);

    if (resenc == NULL) resenc = rb_default_external_encoding();
    if (!rb_enc_asciicompat(resenc)) resenc = rb_usascii_encoding();
    rb_enc_associate(result, resenc);
    str_buf_cat2(result, "\"");

    p = RSTRING_PTR(str); pend = RSTRING_END(str);
    prev = p;
    while (p < pend) {
	unsigned int c, cc;
	int n;

        n = rb_enc_precise_mbclen(p, pend, enc);
        if (!MBCLEN_CHARFOUND_P(n)) {
	    if (p > prev) str_buf_cat(result, prev, p - prev);
	    snprintf(buf, CHAR_ESC_LEN, "\\x%02X", *p & 0377);
	    str_buf_cat(result, buf, strlen(buf));
            prev = ++p;
	    continue;
	}
        n = MBCLEN_CHARFOUND_LEN(n);
	c = rb_enc_mbc_to_codepoint(p, pend, enc);
	p += n;
	if (c == '"'|| c == '\\' ||
	    (c == '#' &&
             p < pend &&
             MBCLEN_CHARFOUND_P(rb_enc_precise_mbclen(p,pend,enc)) &&
             (cc = rb_enc_codepoint(p,pend,enc),
              (cc == '$' || cc == '@' || cc == '{')))) {
	    if (p - n > prev) str_buf_cat(result, prev, p - n - prev);
	    str_buf_cat2(result, "\\");
	    prev = p - n;
	    continue;
	}
	switch (c) {
	  case '\n': cc = 'n'; break;
	  case '\r': cc = 'r'; break;
	  case '\t': cc = 't'; break;
	  case '\f': cc = 'f'; break;
	  case '\013': cc = 'v'; break;
	  case '\010': cc = 'b'; break;
	  case '\007': cc = 'a'; break;
	  case 033: cc = 'e'; break;
	  default: cc = 0; break;
	}
	if (cc) {
	    if (p - n > prev) str_buf_cat(result, prev, p - n - prev);
	    buf[0] = '\\';
	    buf[1] = (char)cc;
	    str_buf_cat(result, buf, 2);
	    prev = p;
	    continue;
	}
	if ((enc == resenc && rb_enc_isprint(c, enc)) ||
	    (rb_enc_isascii(c, enc) && ISPRINT(c))) {
	    continue;
	}
	else {
	    if (p - n > prev) str_buf_cat(result, prev, p - n - prev);
	    if (unicode_p) {
		if (c < 0x10000) {
		    snprintf(buf, CHAR_ESC_LEN, "\\u%04X", c);
		}
		else {
		    snprintf(buf, CHAR_ESC_LEN, "\\u{%X}", c);
		}
		str_buf_cat(result, buf, strlen(buf));
	    }
	    else {
		if (c < 0x100) {
		    snprintf(buf, CHAR_ESC_LEN, "\\x%02X", c);
		}
		else {
		    snprintf(buf, CHAR_ESC_LEN, "\\x{%X}", c);
		}
		str_buf_cat(result, buf, strlen(buf));
	    }
	    prev = p;
	    continue;
	}
    }
    if (p > prev) str_buf_cat(result, prev, p - prev);
    str_buf_cat2(result, "\"");

    OBJ_INFECT(result, str);
    return result;
}