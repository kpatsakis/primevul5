sym_cmp(VALUE sym, VALUE other)
{
    if (!SYMBOL_P(other)) {
	return Qnil;
    }
    return rb_str_cmp_m(rb_sym_to_s(sym), rb_sym_to_s(other));
}