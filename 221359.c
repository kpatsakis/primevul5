rb_str_start_with(int argc, VALUE *argv, VALUE str)
{
    int i;

    for (i=0; i<argc; i++) {
	VALUE tmp = rb_check_string_type(argv[i]);
	if (NIL_P(tmp)) continue;
	rb_enc_check(str, tmp);
	if (RSTRING_LEN(str) < RSTRING_LEN(tmp)) continue;
	if (memcmp(RSTRING_PTR(str), RSTRING_PTR(tmp), RSTRING_LEN(tmp)) == 0)
	    return Qtrue;
    }
    return Qfalse;
}