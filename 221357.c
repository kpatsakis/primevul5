rb_str_split(VALUE str, const char *sep0)
{
    VALUE sep;

    StringValue(str);
    sep = rb_str_new2(sep0);
    return rb_str_split_m(1, &sep, str);
}