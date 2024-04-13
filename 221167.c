rb_str_coderange_scan_restartable(const char *s, const char *e, rb_encoding *enc, int *cr)
{
    const char *p = s;

    if (*cr == ENC_CODERANGE_BROKEN)
	return e - s;

    if (rb_enc_to_index(enc) == 0) {
	/* enc is ASCII-8BIT.  ASCII-8BIT string never be broken. */
	p = search_nonascii(p, e);
	*cr = (!p && *cr != ENC_CODERANGE_VALID) ? ENC_CODERANGE_7BIT : ENC_CODERANGE_VALID;
	return e - s;
    }
    else if (rb_enc_asciicompat(enc)) {
	p = search_nonascii(p, e);
	if (!p) {
	    if (*cr != ENC_CODERANGE_VALID) *cr = ENC_CODERANGE_7BIT;
	    return e - s;
	}
	while (p < e) {
	    int ret = rb_enc_precise_mbclen(p, e, enc);
	    if (!MBCLEN_CHARFOUND_P(ret)) {
		*cr = MBCLEN_INVALID_P(ret) ? ENC_CODERANGE_BROKEN: ENC_CODERANGE_UNKNOWN;
		return p - s;
	    }
	    p += MBCLEN_CHARFOUND_LEN(ret);
	    if (p < e) {
		p = search_nonascii(p, e);
		if (!p) {
		    *cr = ENC_CODERANGE_VALID;
		    return e - s;
		}
	    }
	}
	*cr = e < p ? ENC_CODERANGE_BROKEN: ENC_CODERANGE_VALID;
	return p - s;
    }
    else {
	while (p < e) {
	    int ret = rb_enc_precise_mbclen(p, e, enc);
	    if (!MBCLEN_CHARFOUND_P(ret)) {
		*cr = MBCLEN_INVALID_P(ret) ? ENC_CODERANGE_BROKEN: ENC_CODERANGE_UNKNOWN;
		return p - s;
	    }
	    p += MBCLEN_CHARFOUND_LEN(ret);
	}
	*cr = e < p ? ENC_CODERANGE_BROKEN: ENC_CODERANGE_VALID;
	return p - s;
    }
}