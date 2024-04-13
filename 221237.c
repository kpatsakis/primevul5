rb_str_chop_bang(VALUE str)
{
    str_modify_keep_cr(str);
    if (RSTRING_LEN(str) > 0) {
	long len;
	len = chopped_length(str);
	STR_SET_LEN(str, len);
	RSTRING_PTR(str)[len] = '\0';
	if (ENC_CODERANGE(str) != ENC_CODERANGE_7BIT) {
	    ENC_CODERANGE_CLEAR(str);
	}
	return str;
    }
    return Qnil;
}