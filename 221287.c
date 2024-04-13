rb_str_s_try_convert(VALUE dummy, VALUE str)
{
    return rb_check_string_type(str);
}