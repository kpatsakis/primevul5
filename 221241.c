rb_str_update(VALUE str, long beg, long len, VALUE val)
{
    rb_str_splice(str, beg, len, val);
}