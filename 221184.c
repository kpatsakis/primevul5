sym_swapcase(VALUE sym)
{
    return rb_str_intern(rb_str_swapcase(rb_id2str(SYM2ID(sym))));
}