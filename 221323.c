rb_str_swapcase(VALUE str)
{
    str = rb_str_dup(str);
    rb_str_swapcase_bang(str);
    return str;
}