rb_string_value_ptr(volatile VALUE *ptr)
{
    VALUE str = rb_string_value(ptr);
    return RSTRING_PTR(str);
}