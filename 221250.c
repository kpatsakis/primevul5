rb_str_lstrip_bang(VALUE str)
{
    rb_encoding *enc;
    char *s, *t, *e;

    str_modify_keep_cr(str);
    enc = STR_ENC_GET(str);
    s = RSTRING_PTR(str);
    if (!s || RSTRING_LEN(str) == 0) return Qnil;
    e = t = RSTRING_END(str);
    /* remove spaces at head */
    while (s < e) {
	int n;
	unsigned int cc = rb_enc_codepoint_len(s, e, &n, enc);

	if (!rb_isspace(cc)) break;
	s += n;
    }

    if (s > RSTRING_PTR(str)) {
	STR_SET_LEN(str, t-s);
	memmove(RSTRING_PTR(str), s, RSTRING_LEN(str));
	RSTRING_PTR(str)[RSTRING_LEN(str)] = '\0';
	return str;
    }
    return Qnil;
}