sym_encoding(VALUE sym)
{
    return rb_obj_encoding(rb_id2str(SYM2ID(sym)));
}