rb_str_new(const char *ptr, long len)
{
    return str_new(rb_cString, ptr, len);
}