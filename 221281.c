rb_str_each_line(int argc, VALUE *argv, VALUE str)
{
    rb_encoding *enc;
    VALUE rs;
    unsigned int newline;
    const char *p, *pend, *s, *ptr;
    long len, rslen;
    VALUE line;
    int n;
    VALUE orig = str;

    if (argc == 0) {
	rs = rb_rs;
    }
    else {
	rb_scan_args(argc, argv, "01", &rs);
    }
    RETURN_ENUMERATOR(str, argc, argv);
    if (NIL_P(rs)) {
	rb_yield(str);
	return orig;
    }
    str = rb_str_new4(str);
    ptr = p = s = RSTRING_PTR(str);
    pend = p + RSTRING_LEN(str);
    len = RSTRING_LEN(str);
    StringValue(rs);
    if (rs == rb_default_rs) {
	enc = rb_enc_get(str);
	while (p < pend) {
	    char *p0;

	    p = memchr(p, '\n', pend - p);
	    if (!p) break;
	    p0 = rb_enc_left_char_head(s, p, pend, enc);
	    if (!rb_enc_is_newline(p0, pend, enc)) {
		p++;
		continue;
	    }
	    p = p0 + rb_enc_mbclen(p0, pend, enc);
	    line = rb_str_new5(str, s, p - s);
	    OBJ_INFECT(line, str);
	    rb_enc_cr_str_copy_for_substr(line, str);
	    rb_yield(line);
	    str_mod_check(str, ptr, len);
	    s = p;
	}
	goto finish;
    }

    enc = rb_enc_check(str, rs);
    rslen = RSTRING_LEN(rs);
    if (rslen == 0) {
	newline = '\n';
    }
    else {
	newline = rb_enc_codepoint(RSTRING_PTR(rs), RSTRING_END(rs), enc);
    }

    while (p < pend) {
	unsigned int c = rb_enc_codepoint_len(p, pend, &n, enc);

      again:
	if (rslen == 0 && c == newline) {
	    p += n;
	    if (p < pend && (c = rb_enc_codepoint_len(p, pend, &n, enc)) != newline) {
		goto again;
	    }
	    while (p < pend && rb_enc_codepoint(p, pend, enc) == newline) {
		p += n;
	    }
	    p -= n;
	}
	if (c == newline &&
	    (rslen <= 1 || memcmp(RSTRING_PTR(rs), p, rslen) == 0)) {
	    line = rb_str_new5(str, s, p - s + (rslen ? rslen : n));
	    OBJ_INFECT(line, str);
	    rb_enc_cr_str_copy_for_substr(line, str);
	    rb_yield(line);
	    str_mod_check(str, ptr, len);
	    s = p + (rslen ? rslen : n);
	}
	p += n;
    }

  finish:
    if (s != pend) {
	line = rb_str_new5(str, s, pend - s);
	OBJ_INFECT(line, str);
	rb_enc_cr_str_copy_for_substr(line, str);
	rb_yield(line);
    }

    return orig;
}