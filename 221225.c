rb_str_match(VALUE x, VALUE y)
{
    switch (TYPE(y)) {
      case T_STRING:
	rb_raise(rb_eTypeError, "type mismatch: String given");

      case T_REGEXP:
	return rb_reg_match(y, x);

      default:
	return rb_funcall(y, rb_intern("=~"), 1, x);
    }
}