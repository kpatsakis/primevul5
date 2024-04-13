rb_str_substr(VALUE str, long beg, long len)
{
    rb_encoding *enc = STR_ENC_GET(str);
    VALUE str2;
    char *p, *s = RSTRING_PTR(str), *e = s + RSTRING_LEN(str);

    if (len < 0) return Qnil;
    if (!RSTRING_LEN(str)) {
	len = 0;
    }
    if (single_byte_optimizable(str)) {
	if (beg > RSTRING_LEN(str)) return Qnil;
	if (beg < 0) {
	    beg += RSTRING_LEN(str);
	    if (beg < 0) return Qnil;
	}
	if (beg + len > RSTRING_LEN(str))
	    len = RSTRING_LEN(str) - beg;
	if (len <= 0) {
	    len = 0;
	    p = 0;
	}
	else
	    p = s + beg;
	goto sub;
    }
    if (beg < 0) {
	if (len > -beg) len = -beg;
	if (-beg * rb_enc_mbmaxlen(enc) < RSTRING_LEN(str) / 8) {
	    beg = -beg;
	    while (beg-- > len && (e = rb_enc_prev_char(s, e, e, enc)) != 0);
	    p = e;
	    if (!p) return Qnil;
	    while (len-- > 0 && (p = rb_enc_prev_char(s, p, e, enc)) != 0);
	    if (!p) return Qnil;
	    len = e - p;
	    goto sub;
	}
	else {
	    beg += str_strlen(str, enc);
	    if (beg < 0) return Qnil;
	}
    }
    else if (beg > 0 && beg > str_strlen(str, enc)) {
	return Qnil;
    }
    if (len == 0) {
	p = 0;
    }
#ifdef NONASCII_MASK
    else if (ENC_CODERANGE(str) == ENC_CODERANGE_VALID &&
        enc == rb_utf8_encoding()) {
        p = str_utf8_nth(s, e, beg);
        len = str_utf8_offset(p, e, len);
    }
#endif
    else if (rb_enc_mbmaxlen(enc) == rb_enc_mbminlen(enc)) {
	int char_sz = rb_enc_mbmaxlen(enc);

	p = s + beg * char_sz;
	if (p > e) {
	    p = e;
	    len = 0;
	}
        else if (len * char_sz > e - p)
            len = e - p;
        else
	    len *= char_sz;
    }
    else if ((p = str_nth(s, e, beg, enc, 0)) == e) {
	len = 0;
    }
    else {
	len = str_offset(p, e, len, enc, 0);
    }
  sub:
    if (len > RSTRING_EMBED_LEN_MAX && beg + len == RSTRING_LEN(str)) {
	str2 = rb_str_new4(str);
	str2 = str_new3(rb_obj_class(str2), str2);
	RSTRING(str2)->as.heap.ptr += RSTRING(str2)->as.heap.len - len;
	RSTRING(str2)->as.heap.len = len;
    }
    else {
	str2 = rb_str_new5(str, p, len);
	rb_enc_cr_str_copy_for_substr(str2, str);
	OBJ_INFECT(str2, str);
    }

    return str2;
}