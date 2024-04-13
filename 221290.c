rb_str_chomp(int argc, VALUE *argv, VALUE str)
{
    str = rb_str_dup(str);
    rb_str_chomp_bang(argc, argv, str);
    return str;
}