rb_str_setbyte(VALUE str, VALUE index, VALUE value)
{
    long pos = NUM2LONG(index);
    int byte = NUM2INT(value);

    rb_str_modify(str);

    if (pos < -RSTRING_LEN(str) || RSTRING_LEN(str) <= pos)
        rb_raise(rb_eIndexError, "index %ld out of string", pos);
    if (pos < 0)
        pos += RSTRING_LEN(str);

    RSTRING_PTR(str)[pos] = byte;

    return value;
}