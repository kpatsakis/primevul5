rb_str_offset(VALUE str, long pos)
{
    return str_offset(RSTRING_PTR(str), RSTRING_END(str), pos,
		      STR_ENC_GET(str), single_byte_optimizable(str));
}