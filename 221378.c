rb_string_value(volatile VALUE *ptr)
{
    VALUE s = *ptr;
    if (TYPE(s) != T_STRING) {
	if (SYMBOL_P(s)) {
	    s = rb_sym_to_s(s);
	}
	else {
	    s = rb_str_to_str(s);
	}
	*ptr = s;
    }
    return s;
}