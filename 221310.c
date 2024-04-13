rb_str_strip(VALUE str)
{
    str = rb_str_dup(str);
    rb_str_strip_bang(str);
    return str;
}