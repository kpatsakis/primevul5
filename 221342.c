rb_str_buf_cat2(VALUE str, const char *ptr)
{
    return rb_str_buf_cat(str, ptr, strlen(ptr));
}