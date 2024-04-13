str_duplicate(VALUE klass, VALUE str)
{
    VALUE dup = str_alloc(klass);
    str_replace(dup, str);
    return dup;
}