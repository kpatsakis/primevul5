rb_str_subpat(VALUE str, VALUE re, VALUE backref)
{
    if (rb_reg_search(re, str, 0, 0) >= 0) {
        VALUE match = rb_backref_get();
        int nth = rb_reg_backref_number(match, backref);
	return rb_reg_nth_match(nth, match);
    }
    return Qnil;
}