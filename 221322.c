rb_tainted_str_new(const char *ptr, long len)
{
    VALUE str = rb_str_new(ptr, len);

    OBJ_TAINT(str);
    return str;
}