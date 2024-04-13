rb_str_sub_bang(int argc, VALUE *argv, VALUE str)
{
    VALUE pat, repl, hash = Qnil;
    int iter = 0;
    int tainted = 0;
    int untrusted = 0;
    long plen;

    if (argc == 1 && rb_block_given_p()) {
	iter = 1;
    }
    else if (argc == 2) {
	repl = argv[1];
	hash = rb_check_convert_type(argv[1], T_HASH, "Hash", "to_hash");
	if (NIL_P(hash)) {
	    StringValue(repl);
	}
	if (OBJ_TAINTED(repl)) tainted = 1;
	if (OBJ_UNTRUSTED(repl)) untrusted = 1;
    }
    else {
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 2)", argc);
    }

    pat = get_pat(argv[0], 1);
    if (rb_reg_search(pat, str, 0, 0) >= 0) {
	rb_encoding *enc;
	int cr = ENC_CODERANGE(str);
	VALUE match = rb_backref_get();
	struct re_registers *regs = RMATCH_REGS(match);
	long beg0 = BEG(0);
	long end0 = END(0);
	char *p, *rp;
	long len, rlen;

	if (iter || !NIL_P(hash)) {
	    p = RSTRING_PTR(str); len = RSTRING_LEN(str);

            if (iter) {
                repl = rb_obj_as_string(rb_yield(rb_reg_nth_match(0, match)));
            }
            else {
                repl = rb_hash_aref(hash, rb_str_subseq(str, beg0, end0 - beg0));
                repl = rb_obj_as_string(repl);
            }
	    str_mod_check(str, p, len);
	    str_frozen_check(str);
	}
	else {
	    repl = rb_reg_regsub(repl, str, regs, pat);
	}
        enc = rb_enc_compatible(str, repl);
        if (!enc) {
            rb_encoding *str_enc = STR_ENC_GET(str);
	    p = RSTRING_PTR(str); len = RSTRING_LEN(str);
	    if (coderange_scan(p, beg0, str_enc) != ENC_CODERANGE_7BIT ||
		coderange_scan(p+end0, len-end0, str_enc) != ENC_CODERANGE_7BIT) {
                rb_raise(rb_eEncCompatError, "incompatible character encodings: %s and %s",
			 rb_enc_name(str_enc),
			 rb_enc_name(STR_ENC_GET(repl)));
            }
            enc = STR_ENC_GET(repl);
        }
	rb_str_modify(str);
	rb_enc_associate(str, enc);
	if (OBJ_TAINTED(repl)) tainted = 1;
	if (OBJ_UNTRUSTED(repl)) untrusted = 1;
	if (ENC_CODERANGE_UNKNOWN < cr && cr < ENC_CODERANGE_BROKEN) {
	    int cr2 = ENC_CODERANGE(repl);
            if (cr2 == ENC_CODERANGE_BROKEN ||
                (cr == ENC_CODERANGE_VALID && cr2 == ENC_CODERANGE_7BIT))
                cr = ENC_CODERANGE_UNKNOWN;
            else
                cr = cr2;
	}
	plen = end0 - beg0;
	rp = RSTRING_PTR(repl); rlen = RSTRING_LEN(repl);
	len = RSTRING_LEN(str);
	if (rlen > plen) {
	    RESIZE_CAPA(str, len + rlen - plen);
	}
	p = RSTRING_PTR(str);
	if (rlen != plen) {
	    memmove(p + beg0 + rlen, p + beg0 + plen, len - beg0 - plen);
	}
	memcpy(p + beg0, rp, rlen);
	len += rlen - plen;
	STR_SET_LEN(str, len);
	RSTRING_PTR(str)[len] = '\0';
	ENC_CODERANGE_SET(str, cr);
	if (tainted) OBJ_TAINT(str);
	if (untrusted) OBJ_UNTRUST(str);

	return str;
    }
    return Qnil;
}