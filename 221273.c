rb_str_capitalize(VALUE str)
{
    str = rb_str_dup(str);
    rb_str_capitalize_bang(str);
    return str;
}