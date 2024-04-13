rb_str_cat(VALUE str, const char *ptr, long len)
{
    if (len < 0) {
	rb_raise(rb_eArgError, "negative string size (or size too big)");
    }
    if (STR_ASSOC_P(str)) {
	rb_str_modify(str);
	if (STR_EMBED_P(str)) str_make_independent(str);
	REALLOC_N(RSTRING(str)->as.heap.ptr, char, RSTRING(str)->as.heap.len+len+1);
	memcpy(RSTRING(str)->as.heap.ptr + RSTRING(str)->as.heap.len, ptr, len);
	RSTRING(str)->as.heap.len += len;
	RSTRING(str)->as.heap.ptr[RSTRING(str)->as.heap.len] = '\0'; /* sentinel */
	return str;
    }

    return rb_str_buf_cat(str, ptr, len);
}