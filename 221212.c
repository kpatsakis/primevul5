rb_str_end_with(int argc, VALUE *argv, VALUE str)
{
    int i;
    char *p, *s, *e;
    rb_encoding *enc;

    for (i=0; i<argc; i++) {
	VALUE tmp = rb_check_string_type(argv[i]);
	if (NIL_P(tmp)) continue;
	enc = rb_enc_check(str, tmp);
	if (RSTRING_LEN(str) < RSTRING_LEN(tmp)) continue;
	p = RSTRING_PTR(str);
        e = p + RSTRING_LEN(str);
	s = e - RSTRING_LEN(tmp);
	if (rb_enc_left_char_head(p, s, e, enc) != s)
	    continue;
	if (memcmp(s, RSTRING_PTR(tmp), RSTRING_LEN(tmp)) == 0)
	    return Qtrue;
    }
    return Qfalse;
}