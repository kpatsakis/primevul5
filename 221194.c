rb_str_rstrip(VALUE str)
{
    str = rb_str_dup(str);
    rb_str_rstrip_bang(str);
    return str;
}