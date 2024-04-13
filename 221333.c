sym_length(VALUE sym)
{
    return rb_str_length(rb_id2str(SYM2ID(sym)));
}