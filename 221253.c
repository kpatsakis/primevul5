rb_str_new_with_class(VALUE obj, const char *ptr, long len)
{
    return str_new(rb_obj_class(obj), ptr, len);
}