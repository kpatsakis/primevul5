rb_str_downcase(VALUE str)
{
    str = rb_str_dup(str);
    rb_str_downcase_bang(str);
    return str;
}