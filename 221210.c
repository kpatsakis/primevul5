rb_str_capitalize_bang(VALUE str)
{
    rb_encoding *enc;
    char *s, *send;
    int modify = 0;
    unsigned int c;
    int n;

    str_modify_keep_cr(str);
    enc = STR_ENC_GET(str);
    rb_str_check_dummy_enc(enc);
    if (RSTRING_LEN(str) == 0 || !RSTRING_PTR(str)) return Qnil;
    s = RSTRING_PTR(str); send = RSTRING_END(str);

    c = rb_enc_codepoint_len(s, send, &n, enc);
    if (rb_enc_islower(c, enc)) {
	rb_enc_mbcput(rb_enc_toupper(c, enc), s, enc);
	modify = 1;
    }
    s += n;
    while (s < send) {
	c = rb_enc_codepoint_len(s, send, &n, enc);
	if (rb_enc_isupper(c, enc)) {
	    rb_enc_mbcput(rb_enc_tolower(c, enc), s, enc);
	    modify = 1;
	}
	s += n;
    }

    if (modify) return str;
    return Qnil;
}