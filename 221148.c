rb_str_empty(VALUE str)
{
    if (RSTRING_LEN(str) == 0)
	return Qtrue;
    return Qfalse;
}