rb_str_to_f(VALUE str)
{
    return DBL2NUM(rb_str_to_dbl(str, FALSE));
}