rb_str_partition(VALUE str, VALUE sep)
{
    long pos;
    int regex = FALSE;

    if (TYPE(sep) == T_REGEXP) {
	pos = rb_reg_search(sep, str, 0, 0);
	regex = TRUE;
    }
    else {
	VALUE tmp;

	tmp = rb_check_string_type(sep);
	if (NIL_P(tmp)) {
	    rb_raise(rb_eTypeError, "type mismatch: %s given",
		     rb_obj_classname(sep));
	}
	sep = tmp;
	pos = rb_str_index(str, sep, 0);
    }
    if (pos < 0) {
      failed:
	return rb_ary_new3(3, str, rb_str_new(0,0),rb_str_new(0,0));
    }
    if (regex) {
	sep = rb_str_subpat(str, sep, INT2FIX(0));
	if (pos == 0 && RSTRING_LEN(sep) == 0) goto failed;
    }
    return rb_ary_new3(3, rb_str_subseq(str, 0, pos),
		          sep,
		          rb_str_subseq(str, pos+RSTRING_LEN(sep),
					     RSTRING_LEN(str)-pos-RSTRING_LEN(sep)));
}