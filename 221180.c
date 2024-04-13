rb_str_resurrect(VALUE str)
{
    return str_replace(str_alloc(rb_cString), str);
}