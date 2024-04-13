rb_tainted_str_new_cstr(const char *ptr)
{
    VALUE str = rb_str_new2(ptr);

    OBJ_TAINT(str);
    return str;
}