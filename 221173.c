tr_setup_table(VALUE str, char stable[256], int first,
	       VALUE *tablep, VALUE *ctablep, rb_encoding *enc)
{
    const unsigned int errc = -1;
    char buf[256];
    struct tr tr;
    unsigned int c;
    VALUE table = 0, ptable = 0;
    int i, l, cflag = 0;

    tr.p = RSTRING_PTR(str); tr.pend = tr.p + RSTRING_LEN(str);
    tr.gen = tr.now = tr.max = 0;

    if (RSTRING_LEN(str) > 1 && rb_enc_ascget(tr.p, tr.pend, &l, enc) == '^') {
	cflag = 1;
	tr.p += l;
    }
    if (first) {
	for (i=0; i<256; i++) {
	    stable[i] = 1;
	}
    }
    for (i=0; i<256; i++) {
	buf[i] = cflag;
    }

    while ((c = trnext(&tr, enc)) != errc) {
	if (c < 256) {
	    buf[c & 0xff] = !cflag;
	}
	else {
	    VALUE key = UINT2NUM(c);

	    if (!table) {
		table = rb_hash_new();
		if (cflag) {
		    ptable = *ctablep;
		    *ctablep = table;
		}
		else {
		    ptable = *tablep;
		    *tablep = table;
		}
	    }
	    if (!ptable || !NIL_P(rb_hash_aref(ptable, key))) {
		rb_hash_aset(table, key, Qtrue);
	    }
	}
    }
    for (i=0; i<256; i++) {
	stable[i] = stable[i] && buf[i];
    }
}