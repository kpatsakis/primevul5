rb_str_dump(VALUE str)
{
    rb_encoding *enc = rb_enc_get(str);
    long len;
    const char *p, *pend;
    char *q, *qend;
    VALUE result;
    int u8 = (enc == rb_utf8_encoding());

    len = 2;			/* "" */
    p = RSTRING_PTR(str); pend = p + RSTRING_LEN(str);
    while (p < pend) {
	unsigned char c = *p++;
	switch (c) {
	  case '"':  case '\\':
	  case '\n': case '\r':
	  case '\t': case '\f':
	  case '\013': case '\010': case '\007': case '\033':
	    len += 2;
	    break;

	  case '#':
	    len += IS_EVSTR(p, pend) ? 2 : 1;
	    break;

	  default:
	    if (ISPRINT(c)) {
		len++;
	    }
	    else {
		if (u8) {	/* \u{NN} */
		    char buf[32];
		    int n = rb_enc_precise_mbclen(p-1, pend, enc);
		    if (MBCLEN_CHARFOUND_P(n)) {
			int cc = rb_enc_mbc_to_codepoint(p-1, pend, enc);
			sprintf(buf, "%x", cc);
			len += strlen(buf)+4;
			p += MBCLEN_CHARFOUND_LEN(n)-1;
			break;
		    }
		}
		len += 4;	/* \xNN */
	    }
	    break;
	}
    }
    if (!rb_enc_asciicompat(enc)) {
	len += 19;		/* ".force_encoding('')" */
	len += strlen(enc->name);
    }

    result = rb_str_new5(str, 0, len);
    p = RSTRING_PTR(str); pend = p + RSTRING_LEN(str);
    q = RSTRING_PTR(result); qend = q + len + 1;

    *q++ = '"';
    while (p < pend) {
	unsigned char c = *p++;

	if (c == '"' || c == '\\') {
	    *q++ = '\\';
	    *q++ = c;
	}
	else if (c == '#') {
	    if (IS_EVSTR(p, pend)) *q++ = '\\';
	    *q++ = '#';
	}
	else if (c == '\n') {
	    *q++ = '\\';
	    *q++ = 'n';
	}
	else if (c == '\r') {
	    *q++ = '\\';
	    *q++ = 'r';
	}
	else if (c == '\t') {
	    *q++ = '\\';
	    *q++ = 't';
	}
	else if (c == '\f') {
	    *q++ = '\\';
	    *q++ = 'f';
	}
	else if (c == '\013') {
	    *q++ = '\\';
	    *q++ = 'v';
	}
	else if (c == '\010') {
	    *q++ = '\\';
	    *q++ = 'b';
	}
	else if (c == '\007') {
	    *q++ = '\\';
	    *q++ = 'a';
	}
	else if (c == '\033') {
	    *q++ = '\\';
	    *q++ = 'e';
	}
	else if (ISPRINT(c)) {
	    *q++ = c;
	}
	else {
	    *q++ = '\\';
	    if (u8) {
		int n = rb_enc_precise_mbclen(p-1, pend, enc) - 1;
		if (MBCLEN_CHARFOUND_P(n)) {
		    int cc = rb_enc_mbc_to_codepoint(p-1, pend, enc);
		    p += n;
		    snprintf(q, qend-q, "u{%x}", cc);
		    q += strlen(q);
		    continue;
		}
	    }
	    snprintf(q, qend-q, "x%02X", c);
	    q += 3;
	}
    }
    *q++ = '"';
    *q = '\0';
    if (!rb_enc_asciicompat(enc)) {
	snprintf(q, qend-q, ".force_encoding(\"%s\")", enc->name);
	enc = rb_ascii8bit_encoding();
    }
    OBJ_INFECT(result, str);
    /* result from dump is ASCII */
    rb_enc_associate(result, enc);
    ENC_CODERANGE_SET(result, ENC_CODERANGE_7BIT);
    return result;
}