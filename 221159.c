rb_str_match_m(int argc, VALUE *argv, VALUE str)
{
    VALUE re, result;
    if (argc < 1)
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
    re = argv[0];
    argv[0] = str;
    result = rb_funcall2(get_pat(re, 0), rb_intern("match"), argc, argv);
    if (!NIL_P(result) && rb_block_given_p()) {
	return rb_yield(result);
    }
    return result;
}