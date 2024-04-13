rb_str_rindex(VALUE str, VALUE sub, long pos)
{
    long len, slen;
    char *s, *sbeg, *e, *t;
    rb_encoding *enc;
    int singlebyte = single_byte_optimizable(str);

    enc = rb_enc_check(str, sub);
    if (is_broken_string(sub)) {
	return -1;
    }
    len = str_strlen(str, enc);
    slen = str_strlen(sub, enc);
    /* substring longer than string */
    if (len < slen) return -1;
    if (len - pos < slen) {
	pos = len - slen;
    }
    if (len == 0) {
	return pos;
    }
    sbeg = RSTRING_PTR(str);
    e = RSTRING_END(str);
    t = RSTRING_PTR(sub);
    slen = RSTRING_LEN(sub);
    for (;;) {
	s = str_nth(sbeg, e, pos, enc, singlebyte);
	if (!s) return -1;
	if (memcmp(s, t, slen) == 0) {
	    return pos;
	}
	if (pos == 0) break;
	pos--;
    }
    return -1;
}