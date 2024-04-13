sym_empty(VALUE sym)
{
    return rb_str_empty(rb_id2str(SYM2ID(sym)));
}