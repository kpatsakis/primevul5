rb_str_slice_bang(int argc, VALUE *argv, VALUE str)
{
    VALUE result;
    VALUE buf[3];
    int i;

    if (argc < 1 || 2 < argc) {
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
    }
    for (i=0; i<argc; i++) {
	buf[i] = argv[i];
    }
    str_modify_keep_cr(str);
    buf[i] = rb_str_new(0,0);
    result = rb_str_aref_m(argc, buf, str);
    if (!NIL_P(result)) {
	rb_str_aset_m(argc+1, buf, str);
    }
    return result;
}