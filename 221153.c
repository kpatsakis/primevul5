rb_str_downcase_bang(VALUE str)
{
    rb_encoding *enc;
    char *s, *send;
    int modify = 0;

    str_modify_keep_cr(str);
    enc = STR_ENC_GET(str);
    rb_str_check_dummy_enc(enc);
    s = RSTRING_PTR(str); send = RSTRING_END(str);
    if (single_byte_optimizable(str)) {
	while (s < send) {
	    unsigned int c = *(unsigned char*)s;

	    if (rb_enc_isascii(c, enc) && 'A' <= c && c <= 'Z') {
		*s = 'a' + (c - 'A');
		modify = 1;
	    }
	    s++;
	}
    }
    else {
	int ascompat = rb_enc_asciicompat(enc);

	while (s < send) {
	    unsigned int c;
	    int n;

	    if (ascompat && (c = *(unsigned char*)s) < 0x80) {
		if (rb_enc_isascii(c, enc) && 'A' <= c && c <= 'Z') {
		    *s = 'a' + (c - 'A');
		    modify = 1;
		}
		s++;
	    }
	    else {
		c = rb_enc_codepoint_len(s, send, &n, enc);
		if (rb_enc_isupper(c, enc)) {
		    /* assuming toupper returns codepoint with same size */
		    rb_enc_mbcput(rb_enc_tolower(c, enc), s, enc);
		    modify = 1;
		}
		s += n;
	    }
	}
    }

    if (modify) return str;
    return Qnil;
}