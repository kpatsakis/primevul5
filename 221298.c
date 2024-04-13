rb_str_rstrip_bang(VALUE str)
{
    rb_encoding *enc;
    char *s, *t, *e;

    str_modify_keep_cr(str);
    enc = STR_ENC_GET(str);
    rb_str_check_dummy_enc(enc);
    s = RSTRING_PTR(str);
    if (!s || RSTRING_LEN(str) == 0) return Qnil;
    t = e = RSTRING_END(str);

    /* remove trailing spaces or '\0's */
    if (single_byte_optimizable(str)) {
	unsigned char c;
	while (s < t && ((c = *(t-1)) == '\0' || ascii_isspace(c))) t--;
    }
    else {
	char *tp;

        while ((tp = rb_enc_prev_char(s, t, e, enc)) != NULL) {
	    unsigned int c = rb_enc_codepoint(tp, e, enc);
	    if (c && !rb_isspace(c)) break;
	    t = tp;
	}
    }
    if (t < e) {
	long len = t-RSTRING_PTR(str);

	STR_SET_LEN(str, len);
	RSTRING_PTR(str)[len] = '\0';
	return str;
    }
    return Qnil;
}