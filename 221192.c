sym_match(VALUE sym, VALUE other)
{
    return rb_str_match(rb_sym_to_s(sym), other);
}