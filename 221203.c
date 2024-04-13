rb_str_upto(int argc, VALUE *argv, VALUE beg)
{
    VALUE end, exclusive;
    VALUE current, after_end;
    ID succ;
    int n, excl, ascii;
    rb_encoding *enc;

    rb_scan_args(argc, argv, "11", &end, &exclusive);
    RETURN_ENUMERATOR(beg, argc, argv);
    excl = RTEST(exclusive);
    CONST_ID(succ, "succ");
    StringValue(end);
    enc = rb_enc_check(beg, end);
    ascii = (is_ascii_string(beg) && is_ascii_string(end));
    /* single character */
    if (RSTRING_LEN(beg) == 1 && RSTRING_LEN(end) == 1 && ascii) {
	char c = RSTRING_PTR(beg)[0];
	char e = RSTRING_PTR(end)[0];

	if (c > e || (excl && c == e)) return beg;
	for (;;) {
	    rb_yield(rb_enc_str_new(&c, 1, enc));
	    if (!excl && c == e) break;
	    c++;
	    if (excl && c == e) break;
	}
	return beg;
    }
    /* both edges are all digits */
    if (ascii && ISDIGIT(RSTRING_PTR(beg)[0]) && ISDIGIT(RSTRING_PTR(end)[0])) {
	char *s, *send;
	VALUE b, e;
	int width;

	s = RSTRING_PTR(beg); send = RSTRING_END(beg);
	width = rb_long2int(send - s);
	while (s < send) {
	    if (!ISDIGIT(*s)) goto no_digits;
	    s++;
	}
	s = RSTRING_PTR(end); send = RSTRING_END(end);
	while (s < send) {
	    if (!ISDIGIT(*s)) goto no_digits;
	    s++;
	}
	b = rb_str_to_inum(beg, 10, FALSE);
	e = rb_str_to_inum(end, 10, FALSE);
	if (FIXNUM_P(b) && FIXNUM_P(e)) {
	    long bi = FIX2LONG(b);
	    long ei = FIX2LONG(e);
	    rb_encoding *usascii = rb_usascii_encoding();

	    while (bi <= ei) {
		if (excl && bi == ei) break;
		rb_yield(rb_enc_sprintf(usascii, "%.*ld", width, bi));
		bi++;
	    }
	}
	else {
	    ID op = excl ? '<' : rb_intern("<=");
	    VALUE args[2], fmt = rb_obj_freeze(rb_usascii_str_new_cstr("%.*d"));

	    args[0] = INT2FIX(width);
	    while (rb_funcall(b, op, 1, e)) {
		args[1] = b;
		rb_yield(rb_str_format(numberof(args), args, fmt));
		b = rb_funcall(b, succ, 0, 0);
	    }
	}
	return beg;
    }
    /* normal case */
  no_digits:
    n = rb_str_cmp(beg, end);
    if (n > 0 || (excl && n == 0)) return beg;

    after_end = rb_funcall(end, succ, 0, 0);
    current = rb_str_dup(beg);
    while (!rb_str_equal(current, after_end)) {
	VALUE next = Qnil;
	if (excl || !rb_str_equal(current, end))
	    next = rb_funcall(current, succ, 0, 0);
	rb_yield(current);
	if (NIL_P(next)) break;
	current = next;
	StringValue(current);
	if (excl && rb_str_equal(current, end)) break;
	if (RSTRING_LEN(current) > RSTRING_LEN(end) || RSTRING_LEN(current) == 0)
	    break;
    }

    return beg;
}