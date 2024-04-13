rb_str_each_byte(VALUE str)
{
    long i;

    RETURN_ENUMERATOR(str, 0, 0);
    for (i=0; i<RSTRING_LEN(str); i++) {
	rb_yield(INT2FIX(RSTRING_PTR(str)[i] & 0xff));
    }
    return str;
}