rb_str_chr(VALUE str)
{
    return rb_str_substr(str, 0, 1);
}