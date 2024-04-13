rb_str_squeeze_bang(int argc, VALUE *argv, VALUE str)
{
    char squeez[256];
    rb_encoding *enc = 0;
    VALUE del = 0, nodel = 0;
    char *s, *send, *t;
    int i, modify = 0;
    int ascompat, singlebyte = single_byte_optimizable(str);
    unsigned int save;

    if (argc == 0) {
	enc = STR_ENC_GET(str);
    }
    else {
	for (i=0; i<argc; i++) {
	    VALUE s = argv[i];

	    StringValue(s);
	    enc = rb_enc_check(str, s);
	    if (singlebyte && !single_byte_optimizable(s))
		singlebyte = 0;
	    tr_setup_table(s, squeez, i==0, &del, &nodel, enc);
	}
    }

    str_modify_keep_cr(str);
    s = t = RSTRING_PTR(str);
    if (!s || RSTRING_LEN(str) == 0) return Qnil;
    send = RSTRING_END(str);
    save = -1;
    ascompat = rb_enc_asciicompat(enc);

    if (singlebyte) {
        while (s < send) {
	    unsigned int c = *(unsigned char*)s++;
	    if (c != save || (argc > 0 && !squeez[c])) {
	        *t++ = save = c;
	    }
	}
    } else {
	while (s < send) {
	    unsigned int c;
	    int clen;

	    if (ascompat && (c = *(unsigned char*)s) < 0x80) {
		if (c != save || (argc > 0 && !squeez[c])) {
		    *t++ = save = c;
		}
		s++;
	    }
	    else {
		c = rb_enc_codepoint_len(s, send, &clen, enc);

		if (c != save || (argc > 0 && !tr_find(c, squeez, del, nodel))) {
		    if (t != s) rb_enc_mbcput(c, t, enc);
		    save = c;
		    t += clen;
		}
		s += clen;
	    }
	}
    }

    *t = '\0';
    if (t - RSTRING_PTR(str) != RSTRING_LEN(str)) {
	STR_SET_LEN(str, t - RSTRING_PTR(str));
	modify = 1;
    }

    if (modify) return str;
    return Qnil;
}