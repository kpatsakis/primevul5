rb_sym_to_s(VALUE sym)
{
    ID id = SYM2ID(sym);

    return str_new3(rb_cString, rb_id2str(id));
}