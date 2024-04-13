sym_capitalize(VALUE sym)
{
    return rb_str_intern(rb_str_capitalize(rb_id2str(SYM2ID(sym))));
}