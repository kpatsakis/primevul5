rb_str_intern(VALUE s)
{
    VALUE str = RB_GC_GUARD(s);
    ID id;

    id = rb_intern_str(str);
    return ID2SYM(id);
}