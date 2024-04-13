rb_str_lstrip(VALUE str)
{
    str = rb_str_dup(str);
    rb_str_lstrip_bang(str);
    return str;
}