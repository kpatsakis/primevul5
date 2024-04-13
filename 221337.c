rb_str_splice_0(VALUE str, long beg, long len, VALUE val)
{
    if (beg == 0 && RSTRING_LEN(val) == 0) {
	rb_str_drop_bytes(str, len);
	OBJ_INFECT(str, val);
	return;
    }

    rb_str_modify(str);
    if (len < RSTRING_LEN(val)) {
	/* expand string */
	RESIZE_CAPA(str, RSTRING_LEN(str) + RSTRING_LEN(val) - len + 1);
    }

    if (RSTRING_LEN(val) != len) {
	memmove(RSTRING_PTR(str) + beg + RSTRING_LEN(val),
		RSTRING_PTR(str) + beg + len,
		RSTRING_LEN(str) - (beg + len));
    }
    if (RSTRING_LEN(val) < beg && len < 0) {
	MEMZERO(RSTRING_PTR(str) + RSTRING_LEN(str), char, -len);
    }
    if (RSTRING_LEN(val) > 0) {
	memmove(RSTRING_PTR(str)+beg, RSTRING_PTR(val), RSTRING_LEN(val));
    }
    STR_SET_LEN(str, RSTRING_LEN(str) + RSTRING_LEN(val) - len);
    if (RSTRING_PTR(str)) {
	RSTRING_PTR(str)[RSTRING_LEN(str)] = '\0';
    }
    OBJ_INFECT(str, val);
}