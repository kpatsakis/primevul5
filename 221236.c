sym_upcase(VALUE sym)
{
    return rb_str_intern(rb_str_upcase(rb_id2str(SYM2ID(sym))));
}