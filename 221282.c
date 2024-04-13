sym_succ(VALUE sym)
{
    return rb_str_intern(rb_str_succ(rb_sym_to_s(sym)));
}