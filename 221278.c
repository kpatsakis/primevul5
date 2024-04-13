rb_str_swapcase_bang(VALUE str)
{
    rb_encoding *enc;
    char *s, *send;
    int modify = 0;
    int n;

    str_modify_keep_cr(str);
    enc = STR_ENC_GET(str);
    rb_str_check_dummy_enc(enc);
    s = RSTRING_PTR(str); send = RSTRING_END(str);
    while (s < send) {
	unsigned int c = rb_enc_codepoint_len(s, send, &n, enc);

	if (rb_enc_isupper(c, enc)) {
	    /* assuming toupper returns codepoint with same size */
	    rb_enc_mbcput(rb_enc_tolower(c, enc), s, enc);
	    modify = 1;
	}
	else if (rb_enc_islower(c, enc)) {
	    /* assuming tolower returns codepoint with same size */
	    rb_enc_mbcput(rb_enc_toupper(c, enc), s, enc);
	    modify = 1;
	}
	s += n;
    }

    if (modify) return str;
    return Qnil;
}