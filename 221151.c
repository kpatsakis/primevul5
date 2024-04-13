str_alloc(VALUE klass)
{
    NEWOBJ(str, struct RString);
    OBJSETUP(str, klass, T_STRING);

    str->as.heap.ptr = 0;
    str->as.heap.len = 0;
    str->as.heap.aux.capa = 0;

    return (VALUE)str;
}