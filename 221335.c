scan_once(VALUE str, VALUE pat, long *start)
{
    VALUE result, match;
    struct re_registers *regs;
    int i;

    if (rb_reg_search(pat, str, *start, 0) >= 0) {
	match = rb_backref_get();
	regs = RMATCH_REGS(match);
	if (BEG(0) == END(0)) {
	    rb_encoding *enc = STR_ENC_GET(str);
	    /*
	     * Always consume at least one character of the input string
	     */
	    if (RSTRING_LEN(str) > END(0))
		*start = END(0)+rb_enc_fast_mbclen(RSTRING_PTR(str)+END(0),
						   RSTRING_END(str), enc);
	    else
		*start = END(0)+1;
	}
	else {
	    *start = END(0);
	}
	if (regs->num_regs == 1) {
	    return rb_reg_nth_match(0, match);
	}
	result = rb_ary_new2(regs->num_regs);
	for (i=1; i < regs->num_regs; i++) {
	    rb_ary_push(result, rb_reg_nth_match(i, match));
	}

	return result;
    }
    return Qnil;
}