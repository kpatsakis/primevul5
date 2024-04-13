rb_str_delete_bang(int argc, VALUE *argv, VALUE str)
{
    char squeez[256];
    rb_encoding *enc = 0;
    char *s, *send, *t;
    VALUE del = 0, nodel = 0;
    int modify = 0;
    int i, ascompat, cr;

    if (RSTRING_LEN(str) == 0 || !RSTRING_PTR(str)) return Qnil;
    if (argc < 1) {
	rb_raise(rb_eArgError, "wrong number of arguments");
    }
    for (i=0; i<argc; i++) {
	VALUE s = argv[i];

	StringValue(s);
	enc = rb_enc_check(str, s);
	tr_setup_table(s, squeez, i==0, &del, &nodel, enc);
    }

    str_modify_keep_cr(str);
    ascompat = rb_enc_asciicompat(enc);
    s = t = RSTRING_PTR(str);
    send = RSTRING_END(str);
    cr = ascompat ? ENC_CODERANGE_7BIT : ENC_CODERANGE_VALID;
    while (s < send) {
	unsigned int c;
	int clen;

	if (ascompat && (c = *(unsigned char*)s) < 0x80) {
	    if (squeez[c]) {
		modify = 1;
	    }
	    else {
		if (t != s) *t = c;
		t++;
	    }
	    s++;
	}
	else {
	    c = rb_enc_codepoint_len(s, send, &clen, enc);

	    if (tr_find(c, squeez, del, nodel)) {
		modify = 1;
	    }
	    else {
		if (t != s) rb_enc_mbcput(c, t, enc);
		t += clen;
		if (cr == ENC_CODERANGE_7BIT) cr = ENC_CODERANGE_VALID;
	    }
	    s += clen;
	}
    }
    *t = '\0';
    STR_SET_LEN(str, t - RSTRING_PTR(str));
    ENC_CODERANGE_SET(str, cr);

    if (modify) return str;
    return Qnil;
}