str_new_shared(VALUE klass, VALUE str)
{
    return str_replace_shared(str_alloc(klass), str);
}