rb_str_subpat_set(VALUE str, VALUE re, VALUE backref, VALUE val)
{
    int nth;
    VALUE match;
    long start, end, len;
    rb_encoding *enc;
    struct re_registers *regs;

    if (rb_reg_search(re, str, 0, 0) < 0) {
	rb_raise(rb_eIndexError, "regexp not matched");
    }
    match = rb_backref_get();
    nth = rb_reg_backref_number(match, backref);
    regs = RMATCH_REGS(match);
    if (nth >= regs->num_regs) {
      out_of_range:
	rb_raise(rb_eIndexError, "index %d out of regexp", nth);
    }
    if (nth < 0) {
	if (-nth >= regs->num_regs) {
	    goto out_of_range;
	}
	nth += regs->num_regs;
    }

    start = BEG(nth);
    if (start == -1) {
	rb_raise(rb_eIndexError, "regexp group %d not matched", nth);
    }
    end = END(nth);
    len = end - start;
    StringValue(val);
    enc = rb_enc_check(str, val);
    rb_str_splice_0(str, start, len, val);
    rb_enc_associate(str, enc);
}