rb_str_scan(VALUE str, VALUE pat)
{
    VALUE result;
    long start = 0;
    long last = -1, prev = 0;
    char *p = RSTRING_PTR(str); long len = RSTRING_LEN(str);

    pat = get_pat(pat, 1);
    if (!rb_block_given_p()) {
	VALUE ary = rb_ary_new();

	while (!NIL_P(result = scan_once(str, pat, &start))) {
	    last = prev;
	    prev = start;
	    rb_ary_push(ary, result);
	}
	if (last >= 0) rb_reg_search(pat, str, last, 0);
	return ary;
    }

    while (!NIL_P(result = scan_once(str, pat, &start))) {
	last = prev;
	prev = start;
	rb_yield(result);
	str_mod_check(str, p, len);
    }
    if (last >= 0) rb_reg_search(pat, str, last, 0);
    return str;
}