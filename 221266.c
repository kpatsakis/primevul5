sym_inspect(VALUE sym)
{
    VALUE str;
    ID id = SYM2ID(sym);
    rb_encoding *enc;

    sym = rb_id2str(id);
    enc = STR_ENC_GET(sym);
    str = rb_enc_str_new(0, RSTRING_LEN(sym)+1, enc);
    RSTRING_PTR(str)[0] = ':';
    memcpy(RSTRING_PTR(str)+1, RSTRING_PTR(sym), RSTRING_LEN(sym));
    if (RSTRING_LEN(sym) != (long)strlen(RSTRING_PTR(sym)) ||
	!rb_enc_symname_p(RSTRING_PTR(sym), enc) ||
	!sym_printable(RSTRING_PTR(sym), RSTRING_END(sym), enc)) {
	str = rb_str_inspect(str);
	memcpy(RSTRING_PTR(str), ":\"", 2);
    }
    return str;
}