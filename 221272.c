rb_str_modify(VALUE str)
{
    if (!str_independent(str))
	str_make_independent(str);
    ENC_CODERANGE_CLEAR(str);
}