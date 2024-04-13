rb_str_index(VALUE str, VALUE sub, long offset)
{
    long pos;
    char *s, *sptr, *e;
    long len, slen;
    rb_encoding *enc;

    enc = rb_enc_check(str, sub);
    if (is_broken_string(sub)) {
	return -1;
    }
    len = str_strlen(str, enc);
    slen = str_strlen(sub, enc);
    if (offset < 0) {
	offset += len;
	if (offset < 0) return -1;
    }
    if (len - offset < slen) return -1;
    s = RSTRING_PTR(str);
    e = s + RSTRING_LEN(str);
    if (offset) {
	offset = str_offset(s, RSTRING_END(str), offset, enc, single_byte_optimizable(str));
	s += offset;
    }
    if (slen == 0) return offset;
    /* need proceed one character at a time */
    sptr = RSTRING_PTR(sub);
    slen = RSTRING_LEN(sub);
    len = RSTRING_LEN(str) - offset;
    for (;;) {
	char *t;
	pos = rb_memsearch(sptr, slen, s, len, enc);
	if (pos < 0) return pos;
	t = rb_enc_right_char_head(s, s+pos, e, enc);
	if (t == s + pos) break;
	if ((len -= t - s) <= 0) return -1;
	offset += t - s;
	s = t;
    }
    return pos + offset;
}