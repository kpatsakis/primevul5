rb_str_strip_bang(VALUE str)
{
    VALUE l = rb_str_lstrip_bang(str);
    VALUE r = rb_str_rstrip_bang(str);

    if (NIL_P(l) && NIL_P(r)) return Qnil;
    return str;
}