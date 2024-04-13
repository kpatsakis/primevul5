rb_string_value_cstr(volatile VALUE *ptr)
{
    VALUE str = rb_string_value(ptr);
    char *s = RSTRING_PTR(str);

    if (!s || RSTRING_LEN(str) != (long)strlen(s)) {
	rb_raise(rb_eArgError, "string contains null byte");
    }
    return s;
}