str_gsub(int argc, VALUE *argv, VALUE str, int bang)
{
    VALUE pat, val, repl, match, dest, hash = Qnil;
    struct re_registers *regs;
    long beg, n;
    long beg0, end0;
    long offset, blen, slen, len, last;
    int iter = 0;
    char *sp, *cp;
    int tainted = 0;
    rb_encoding *str_enc;

    switch (argc) {
      case 1:
	RETURN_ENUMERATOR(str, argc, argv);
	iter = 1;
	break;
      case 2:
	repl = argv[1];
	hash = rb_check_convert_type(argv[1], T_HASH, "Hash", "to_hash");
	if (NIL_P(hash)) {
	    StringValue(repl);
	}
	if (OBJ_TAINTED(repl)) tainted = 1;
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 2)", argc);
    }

    pat = get_pat(argv[0], 1);
    beg = rb_reg_search(pat, str, 0, 0);
    if (beg < 0) {
	if (bang) return Qnil;	/* no match, no substitution */
	return rb_str_dup(str);
    }

    offset = 0;
    n = 0;
    blen = RSTRING_LEN(str) + 30; /* len + margin */
    dest = rb_str_buf_new(blen);
    sp = RSTRING_PTR(str);
    slen = RSTRING_LEN(str);
    cp = sp;
    str_enc = STR_ENC_GET(str);

    do {
	n++;
	match = rb_backref_get();
	regs = RMATCH_REGS(match);
	beg0 = BEG(0);
	end0 = END(0);
	if (iter || !NIL_P(hash)) {
            if (iter) {
                val = rb_obj_as_string(rb_yield(rb_reg_nth_match(0, match)));
            }
            else {
                val = rb_hash_aref(hash, rb_str_subseq(str, BEG(0), END(0) - BEG(0)));
                val = rb_obj_as_string(val);
            }
	    str_mod_check(str, sp, slen);
	    if (val == dest) { 	/* paranoid check [ruby-dev:24827] */
		rb_raise(rb_eRuntimeError, "block should not cheat");
	    }
	}
	else {
	    val = rb_reg_regsub(repl, str, regs, pat);
	}

	if (OBJ_TAINTED(val)) tainted = 1;

	len = beg - offset;	/* copy pre-match substr */
        if (len) {
            rb_enc_str_buf_cat(dest, cp, len, str_enc);
        }

        rb_str_buf_append(dest, val);

	last = offset;
	offset = end0;
	if (beg0 == end0) {
	    /*
	     * Always consume at least one character of the input string
	     * in order to prevent infinite loops.
	     */
	    if (RSTRING_LEN(str) <= end0) break;
	    len = rb_enc_fast_mbclen(RSTRING_PTR(str)+end0, RSTRING_END(str), str_enc);
            rb_enc_str_buf_cat(dest, RSTRING_PTR(str)+end0, len, str_enc);
	    offset = end0 + len;
	}
	cp = RSTRING_PTR(str) + offset;
	if (offset > RSTRING_LEN(str)) break;
	beg = rb_reg_search(pat, str, offset, 0);
    } while (beg >= 0);
    if (RSTRING_LEN(str) > offset) {
        rb_enc_str_buf_cat(dest, cp, RSTRING_LEN(str) - offset, str_enc);
    }
    rb_reg_search(pat, str, last, 0);
    if (bang) {
        rb_str_shared_replace(str, dest);
    }
    else {
	RBASIC(dest)->klass = rb_obj_class(str);
	OBJ_INFECT(dest, str);
	str = dest;
    }

    if (tainted) OBJ_TAINT(str);
    return str;
}