tr_trans(VALUE str, VALUE src, VALUE repl, int sflag)
{
    const unsigned int errc = -1;
    unsigned int trans[256];
    rb_encoding *enc, *e1, *e2;
    struct tr trsrc, trrepl;
    int cflag = 0;
    unsigned int c, c0;
    int last = 0, modify = 0, i, l;
    char *s, *send;
    VALUE hash = 0;
    int singlebyte = single_byte_optimizable(str);
    int cr;

#define CHECK_IF_ASCII(c) \
    (void)((cr == ENC_CODERANGE_7BIT && !rb_isascii(c)) ? \
	   (cr = ENC_CODERANGE_VALID) : 0)

    StringValue(src);
    StringValue(repl);
    if (RSTRING_LEN(str) == 0 || !RSTRING_PTR(str)) return Qnil;
    if (RSTRING_LEN(repl) == 0) {
	return rb_str_delete_bang(1, &src, str);
    }

    cr = ENC_CODERANGE(str);
    e1 = rb_enc_check(str, src);
    e2 = rb_enc_check(str, repl);
    if (e1 == e2) {
	enc = e1;
    }
    else {
	enc = rb_enc_check(src, repl);
    }
    trsrc.p = RSTRING_PTR(src); trsrc.pend = trsrc.p + RSTRING_LEN(src);
    if (RSTRING_LEN(src) > 1 &&
	rb_enc_ascget(trsrc.p, trsrc.pend, &l, enc) == '^' &&
	trsrc.p + l < trsrc.pend) {
	cflag = 1;
	trsrc.p += l;
    }
    trrepl.p = RSTRING_PTR(repl);
    trrepl.pend = trrepl.p + RSTRING_LEN(repl);
    trsrc.gen = trrepl.gen = 0;
    trsrc.now = trrepl.now = 0;
    trsrc.max = trrepl.max = 0;

    if (cflag) {
	for (i=0; i<256; i++) {
	    trans[i] = 1;
	}
	while ((c = trnext(&trsrc, enc)) != errc) {
	    if (c < 256) {
		trans[c] = errc;
	    }
	    else {
		if (!hash) hash = rb_hash_new();
		rb_hash_aset(hash, UINT2NUM(c), Qtrue);
	    }
	}
	while ((c = trnext(&trrepl, enc)) != errc)
	    /* retrieve last replacer */;
	last = trrepl.now;
	for (i=0; i<256; i++) {
	    if (trans[i] != errc) {
		trans[i] = last;
	    }
	}
    }
    else {
	unsigned int r;

	for (i=0; i<256; i++) {
	    trans[i] = errc;
	}
	while ((c = trnext(&trsrc, enc)) != errc) {
	    r = trnext(&trrepl, enc);
	    if (r == errc) r = trrepl.now;
	    if (c < 256) {
		trans[c] = r;
		if (rb_enc_codelen(r, enc) != 1) singlebyte = 0;
	    }
	    else {
		if (!hash) hash = rb_hash_new();
		rb_hash_aset(hash, UINT2NUM(c), UINT2NUM(r));
	    }
	}
    }

    if (cr == ENC_CODERANGE_VALID)
	cr = ENC_CODERANGE_7BIT;
    str_modify_keep_cr(str);
    s = RSTRING_PTR(str); send = RSTRING_END(str);
    if (sflag) {
	int clen, tlen;
	long offset, max = RSTRING_LEN(str);
	unsigned int save = -1;
	char *buf = ALLOC_N(char, max), *t = buf;

	while (s < send) {
	    int may_modify = 0;

	    c0 = c = rb_enc_codepoint_len(s, send, &clen, e1);
	    tlen = enc == e1 ? clen : rb_enc_codelen(c, enc);

	    s += clen;
	    if (c < 256) {
		c = trans[c];
	    }
	    else if (hash) {
		VALUE tmp = rb_hash_lookup(hash, UINT2NUM(c));
		if (NIL_P(tmp)) {
		    if (cflag) c = last;
		    else c = errc;
		}
		else if (cflag) c = errc;
		else c = NUM2INT(tmp);
	    }
	    else {
		c = errc;
	    }
	    if (c != (unsigned int)-1) {
		if (save == c) {
		    CHECK_IF_ASCII(c);
		    continue;
		}
		save = c;
		tlen = rb_enc_codelen(c, enc);
		modify = 1;
	    }
	    else {
		save = -1;
		c = c0;
		if (enc != e1) may_modify = 1;
	    }
	    while (t - buf + tlen >= max) {
		offset = t - buf;
		max *= 2;
		REALLOC_N(buf, char, max);
		t = buf + offset;
	    }
	    rb_enc_mbcput(c, t, enc);
	    if (may_modify && memcmp(s, t, tlen) != 0) {
		modify = 1;
	    }
	    CHECK_IF_ASCII(c);
	    t += tlen;
	}
	*t = '\0';
	RSTRING(str)->as.heap.ptr = buf;
	RSTRING(str)->as.heap.len = t - buf;
	STR_SET_NOEMBED(str);
	RSTRING(str)->as.heap.aux.capa = max;
    }
    else if (rb_enc_mbmaxlen(enc) == 1 || (singlebyte && !hash)) {
	while (s < send) {
	    c = (unsigned char)*s;
	    if (trans[c] != errc) {
		if (!cflag) {
		    c = trans[c];
		    *s = c;
		    modify = 1;
		}
		else {
		    *s = last;
		    modify = 1;
		}
	    }
	    CHECK_IF_ASCII(c);
	    s++;
	}
    }
    else {
	int clen, tlen, max = (int)(RSTRING_LEN(str) * 1.2);
	long offset;
	char *buf = ALLOC_N(char, max), *t = buf;

	while (s < send) {
	    int may_modify = 0;
	    c0 = c = rb_enc_codepoint_len(s, send, &clen, e1);
	    tlen = enc == e1 ? clen : rb_enc_codelen(c, enc);

	    if (c < 256) {
		c = trans[c];
	    }
	    else if (hash) {
		VALUE tmp = rb_hash_lookup(hash, UINT2NUM(c));
		if (NIL_P(tmp)) {
		    if (cflag) c = last;
		    else c = errc;
		}
		else if (cflag) c = errc;
		else c = NUM2INT(tmp);
	    }
	    else {
		c = errc;
	    }
	    if (c != errc) {
		tlen = rb_enc_codelen(c, enc);
		modify = 1;
	    }
	    else {
		c = c0;
		if (enc != e1) may_modify = 1;
	    }
	    while (t - buf + tlen >= max) {
		offset = t - buf;
		max *= 2;
		REALLOC_N(buf, char, max);
		t = buf + offset;
	    }
	    if (s != t) {
		rb_enc_mbcput(c, t, enc);
		if (may_modify && memcmp(s, t, tlen) != 0) {
		    modify = 1;
		}
	    }
	    CHECK_IF_ASCII(c);
	    s += clen;
	    t += tlen;
	}
	if (!STR_EMBED_P(str)) {
	    xfree(RSTRING(str)->as.heap.ptr);
	}
	*t = '\0';
	RSTRING(str)->as.heap.ptr = buf;
	RSTRING(str)->as.heap.len = t - buf;
	STR_SET_NOEMBED(str);
	RSTRING(str)->as.heap.aux.capa = max;
    }

    if (modify) {
	if (cr != ENC_CODERANGE_BROKEN)
	    ENC_CODERANGE_SET(str, cr);
	rb_enc_associate(str, enc);
	return str;
    }
    return Qnil;
}