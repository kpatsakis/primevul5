rb_str_succ_bang(VALUE str)
{
    rb_str_shared_replace(str, rb_str_succ(str));

    return str;
}