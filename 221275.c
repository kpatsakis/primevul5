sym_downcase(VALUE sym)
{
    return rb_str_intern(rb_str_downcase(rb_id2str(SYM2ID(sym))));
}