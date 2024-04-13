rb_str_chomp_bang(int argc, VALUE *argv, VALUE str)
{
    rb_encoding *enc;
    VALUE rs;
    int newline;
    char *p, *pp, *e;
    long len, rslen;

    str_modify_keep_cr(str);
    len = RSTRING_LEN(str);
    if (len == 0) return Qnil;
    p = RSTRING_PTR(str);
    e = p + len;
    if (argc == 0) {
	rs = rb_rs;
	if (rs == rb_default_rs) {
	  smart_chomp:
	    enc = rb_enc_get(str);
	    if (rb_enc_mbminlen(enc) > 1) {
		pp = rb_enc_left_char_head(p, e-rb_enc_mbminlen(enc), e, enc);
		if (rb_enc_is_newline(pp, e, enc)) {
		    e = pp;
		}
		pp = e - rb_enc_mbminlen(enc);
		if (pp >= p) {
		    pp = rb_enc_left_char_head(p, pp, e, enc);
		    if (rb_enc_ascget(pp, e, 0, enc) == '\r') {
			e = pp;
		    }
		}
		if (e == RSTRING_END(str)) {
		    return Qnil;
		}
		len = e - RSTRING_PTR(str);
		STR_SET_LEN(str, len);
	    }
	    else {
		if (RSTRING_PTR(str)[len-1] == '\n') {
		    STR_DEC_LEN(str);
		    if (RSTRING_LEN(str) > 0 &&
			RSTRING_PTR(str)[RSTRING_LEN(str)-1] == '\r') {
			STR_DEC_LEN(str);
		    }
		}
		else if (RSTRING_PTR(str)[len-1] == '\r') {
		    STR_DEC_LEN(str);
		}
		else {
		    return Qnil;
		}
	    }
	    RSTRING_PTR(str)[RSTRING_LEN(str)] = '\0';
	    return str;
	}
    }
    else {
	rb_scan_args(argc, argv, "01", &rs);
    }
    if (NIL_P(rs)) return Qnil;
    StringValue(rs);
    rslen = RSTRING_LEN(rs);
    if (rslen == 0) {
	while (len>0 && p[len-1] == '\n') {
	    len--;
	    if (len>0 && p[len-1] == '\r')
		len--;
	}
	if (len < RSTRING_LEN(str)) {
	    STR_SET_LEN(str, len);
	    RSTRING_PTR(str)[len] = '\0';
	    return str;
	}
	return Qnil;
    }
    if (rslen > len) return Qnil;
    newline = RSTRING_PTR(rs)[rslen-1];
    if (rslen == 1 && newline == '\n')
	goto smart_chomp;

    enc = rb_enc_check(str, rs);
    if (is_broken_string(rs)) {
	return Qnil;
    }
    pp = e - rslen;
    if (p[len-1] == newline &&
	(rslen <= 1 ||
	 memcmp(RSTRING_PTR(rs), pp, rslen) == 0)) {
	if (rb_enc_left_char_head(p, pp, e, enc) != pp)
	    return Qnil;
	if (ENC_CODERANGE(str) != ENC_CODERANGE_7BIT) {
	    ENC_CODERANGE_CLEAR(str);
	}
	STR_SET_LEN(str, RSTRING_LEN(str) - rslen);
	RSTRING_PTR(str)[RSTRING_LEN(str)] = '\0';
	return str;
    }
    return Qnil;
}