rb_str_rpartition(VALUE str, VALUE sep)
{
    long pos = RSTRING_LEN(str);
    int regex = FALSE;

    if (TYPE(sep) == T_REGEXP) {
	pos = rb_reg_search(sep, str, pos, 1);
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
	pos = rb_str_sublen(str, pos);
	pos = rb_str_rindex(str, sep, pos);
    }
    if (pos < 0) {
	return rb_ary_new3(3, rb_str_new(0,0),rb_str_new(0,0), str);
    }
    if (regex) {
	sep = rb_reg_nth_match(0, rb_backref_get());
    }
    return rb_ary_new3(3, rb_str_substr(str, 0, pos),
		          sep,
		          rb_str_substr(str,pos+str_strlen(sep,STR_ENC_GET(sep)),RSTRING_LEN(str)));
}