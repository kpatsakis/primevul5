rb_enc_strlen_cr(const char *p, const char *e, rb_encoding *enc, int *cr)
{
    long c;
    const char *q;
    int ret;

    *cr = 0;
    if (rb_enc_mbmaxlen(enc) == rb_enc_mbminlen(enc)) {
	return (e - p + rb_enc_mbminlen(enc) - 1) / rb_enc_mbminlen(enc);
    }
    else if (rb_enc_asciicompat(enc)) {
	c = 0;
	while (p < e) {
	    if (ISASCII(*p)) {
		q = search_nonascii(p, e);
		if (!q) {
		    if (!*cr) *cr = ENC_CODERANGE_7BIT;
		    return c + (e - p);
		}
		c += q - p;
		p = q;
	    }
	    ret = rb_enc_precise_mbclen(p, e, enc);
	    if (MBCLEN_CHARFOUND_P(ret)) {
		*cr |= ENC_CODERANGE_VALID;
		p += MBCLEN_CHARFOUND_LEN(ret);
	    }
	    else {
		*cr = ENC_CODERANGE_BROKEN;
		p++;
	    }
	    c++;
	}
	if (!*cr) *cr = ENC_CODERANGE_7BIT;
	return c;
    }

    for (c=0; p<e; c++) {
	ret = rb_enc_precise_mbclen(p, e, enc);
	if (MBCLEN_CHARFOUND_P(ret)) {
	    *cr |= ENC_CODERANGE_VALID;
	    p += MBCLEN_CHARFOUND_LEN(ret);
	}
	else {
	    *cr = ENC_CODERANGE_BROKEN;
	    p++;
	}
    }
    if (!*cr) *cr = ENC_CODERANGE_7BIT;
    return c;
}