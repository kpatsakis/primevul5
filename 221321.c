rb_str_cat2(VALUE str, const char *ptr)
{
    return rb_str_cat(str, ptr, strlen(ptr));
}