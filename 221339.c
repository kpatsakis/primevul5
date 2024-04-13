rb_str_getbyte(VALUE str, VALUE index)
{
    long pos = NUM2LONG(index);

    if (pos < 0)
        pos += RSTRING_LEN(str);
    if (pos < 0 ||  RSTRING_LEN(str) <= pos)
        return Qnil;

    return INT2FIX((unsigned char)RSTRING_PTR(str)[pos]);
}