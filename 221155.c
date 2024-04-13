str_mod_check(VALUE s, const char *p, long len)
{
    if (RSTRING_PTR(s) != p || RSTRING_LEN(s) != len){
	rb_raise(rb_eRuntimeError, "string modified");
    }
}