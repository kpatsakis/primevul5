rb_str_aset(VALUE str, VALUE indx, VALUE val)
{
    long idx, beg;

    switch (TYPE(indx)) {
      case T_FIXNUM:
	idx = FIX2LONG(indx);
      num_index:
	rb_str_splice(str, idx, 1, val);
	return val;

      case T_REGEXP:
	rb_str_subpat_set(str, indx, INT2FIX(0), val);
	return val;

      case T_STRING:
	beg = rb_str_index(str, indx, 0);
	if (beg < 0) {
	    rb_raise(rb_eIndexError, "string not matched");
	}
	beg = rb_str_sublen(str, beg);
	rb_str_splice(str, beg, str_strlen(indx, 0), val);
	return val;

      default:
	/* check if indx is Range */
	{
	    long beg, len;
	    if (rb_range_beg_len(indx, &beg, &len, str_strlen(str, 0), 2)) {
		rb_str_splice(str, beg, len, val);
		return val;
	    }
	}
	idx = NUM2LONG(indx);
	goto num_index;
    }
}