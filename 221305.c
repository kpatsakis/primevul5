rb_str_equal(VALUE str1, VALUE str2)
{
    if (str1 == str2) return Qtrue;
    if (TYPE(str2) != T_STRING) {
	if (!rb_respond_to(str2, rb_intern("to_str"))) {
	    return Qfalse;
	}
	return rb_equal(str2, str1);
    }
    return str_eql(str1, str2);
}