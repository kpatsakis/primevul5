rb_str_count(int argc, VALUE *argv, VALUE str)
{
    char table[256];
    rb_encoding *enc = 0;
    VALUE del = 0, nodel = 0;
    char *s, *send;
    int i;
    int ascompat;

    if (argc < 1) {
	rb_raise(rb_eArgError, "wrong number of arguments");
    }
    for (i=0; i<argc; i++) {
	VALUE tstr = argv[i];
	unsigned char c;

	StringValue(tstr);
	enc = rb_enc_check(str, tstr);
	if (argc == 1 && RSTRING_LEN(tstr) == 1 && rb_enc_asciicompat(enc) &&
	    (c = RSTRING_PTR(tstr)[0]) < 0x80 && !is_broken_string(str)) {
	    int n = 0;

	    s = RSTRING_PTR(str);
	    if (!s || RSTRING_LEN(str) == 0) return INT2FIX(0);
	    send = RSTRING_END(str);
	    while (s < send) {
		if (*(unsigned char*)s++ == c) n++;
	    }
	    return INT2NUM(n);
	}
	tr_setup_table(tstr, table, i==0, &del, &nodel, enc);
    }

    s = RSTRING_PTR(str);
    if (!s || RSTRING_LEN(str) == 0) return INT2FIX(0);
    send = RSTRING_END(str);
    ascompat = rb_enc_asciicompat(enc);
    i = 0;
    while (s < send) {
	unsigned int c;
	int clen;

	if (ascompat && (c = *(unsigned char*)s) < 0x80) {
	    clen = 1;
	    if (table[c]) {
		i++;
	    }
	    s++;
	}
	else {
	    c = rb_enc_codepoint_len(s, send, &clen, enc);
	    if (tr_find(c, table, del, nodel)) {
		i++;
	    }
	    s += clen;
	}
    }

    return INT2NUM(i);
}