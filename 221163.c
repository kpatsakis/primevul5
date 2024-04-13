rb_to_id(VALUE name)
{
    VALUE tmp;
    ID id;

    switch (TYPE(name)) {
      default:
	tmp = rb_check_string_type(name);
	if (NIL_P(tmp)) {
	    tmp = rb_inspect(name);
	    rb_raise(rb_eTypeError, "%s is not a symbol",
		     RSTRING_PTR(tmp));
	}
	name = tmp;
	/* fall through */
      case T_STRING:
	name = rb_str_intern(name);
	/* fall through */
      case T_SYMBOL:
	return SYM2ID(name);
    }
    return id;
}