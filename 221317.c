str_strlen(VALUE str, rb_encoding *enc)
{
    const char *p, *e;
    long n;
    int cr;

    if (single_byte_optimizable(str)) return RSTRING_LEN(str);
    if (!enc) enc = STR_ENC_GET(str);
    p = RSTRING_PTR(str);
    e = RSTRING_END(str);
    cr = ENC_CODERANGE(str);
#ifdef NONASCII_MASK
    if (ENC_CODERANGE(str) == ENC_CODERANGE_VALID &&
        enc == rb_utf8_encoding()) {

	VALUE len = 0;
	if ((int)sizeof(VALUE) * 2 < e - p) {
	    const VALUE *s, *t;
	    const VALUE lowbits = sizeof(VALUE) - 1;
	    s = (const VALUE*)(~lowbits & ((VALUE)p + lowbits));
	    t = (const VALUE*)(~lowbits & (VALUE)e);
	    while (p < (const char *)s) {
		if (is_utf8_lead_byte(*p)) len++;
		p++;
	    }
	    while (s < t) {
		len += count_utf8_lead_bytes_with_word(s);
		s++;
	    }
	    p = (const char *)s;
	}
	while (p < e) {
	    if (is_utf8_lead_byte(*p)) len++;
	    p++;
	}
	return (long)len;
    }
#endif
    n = rb_enc_strlen_cr(p, e, enc, &cr);
    if (cr) {
        ENC_CODERANGE_SET(str, cr);
    }
    return n;
}