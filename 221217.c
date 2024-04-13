rb_str_format_m(VALUE str, VALUE arg)
{
    volatile VALUE tmp = rb_check_array_type(arg);

    if (!NIL_P(tmp)) {
	return rb_str_format(RARRAY_LENINT(tmp), RARRAY_PTR(tmp), str);
    }
    return rb_str_format(1, &arg, str);
}