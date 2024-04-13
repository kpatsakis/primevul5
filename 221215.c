enc_strlen(const char *p, const char *e, rb_encoding *enc, int cr)
{
    long c;
    const char *q;

    if (rb_enc_mbmaxlen(enc) == rb_enc_mbminlen(enc)) {
        return (e - p + rb_enc_mbminlen(enc) - 1) / rb_enc_mbminlen(enc);
    }
    else if (rb_enc_asciicompat(enc)) {
        c = 0;
	if (cr == ENC_CODERANGE_7BIT || cr == ENC_CODERANGE_VALID) {
	    while (p < e) {
		if (ISASCII(*p)) {
		    q = search_nonascii(p, e);
		    if (!q)
			return c + (e - p);
		    c += q - p;
		    p = q;
		}
		p += rb_enc_fast_mbclen(p, e, enc);
		c++;
	    }
	}
	else {
	    while (p < e) {
		if (ISASCII(*p)) {
		    q = search_nonascii(p, e);
		    if (!q)
			return c + (e - p);
		    c += q - p;
		    p = q;
		}
		p += rb_enc_mbclen(p, e, enc);
		c++;
	    }
	}
        return c;
    }

    for (c=0; p<e; c++) {
        p += rb_enc_mbclen(p, e, enc);
    }
    return c;
}