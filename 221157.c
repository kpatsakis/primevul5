rb_str_bytesize(VALUE str)
{
    return INT2NUM(RSTRING_LEN(str));
}