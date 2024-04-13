rb_str_init(int argc, VALUE *argv, VALUE str)
{
    VALUE orig;

    if (argc > 0 && rb_scan_args(argc, argv, "01", &orig) == 1)
	rb_str_replace(str, orig);
    return str;
}