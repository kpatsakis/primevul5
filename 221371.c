sym_call(VALUE args, VALUE sym, int argc, VALUE *argv)
{
    VALUE obj;

    if (argc < 1) {
	rb_raise(rb_eArgError, "no receiver given");
    }
    obj = argv[0];
    return rb_funcall3(obj, (ID)sym, argc - 1, argv + 1);
}