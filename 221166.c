rb_str_new_cstr(const char *ptr)
{
    if (!ptr) {
	rb_raise(rb_eArgError, "NULL pointer given");
    }
    return rb_str_new(ptr, strlen(ptr));
}