get_pat(VALUE pat, int quote)
{
    VALUE val;

    switch (TYPE(pat)) {
      case T_REGEXP:
	return pat;

      case T_STRING:
	break;

      default:
	val = rb_check_string_type(pat);
	if (NIL_P(val)) {
	    Check_Type(pat, T_REGEXP);
	}
	pat = val;
    }

    if (quote) {
	pat = rb_reg_quote(pat);
    }

    return rb_reg_regcomp(pat);
}