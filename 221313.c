rb_str_gsub_bang(int argc, VALUE *argv, VALUE str)
{
    str_modify_keep_cr(str);
    return str_gsub(argc, argv, str, 1);
}