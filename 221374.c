str_buf_cat(VALUE str, const char *ptr, long len)
{
    long capa, total, off = -1;

    if (ptr >= RSTRING_PTR(str) && ptr <= RSTRING_END(str)) {
        off = ptr - RSTRING_PTR(str);
    }
    rb_str_modify(str);
    if (len == 0) return 0;
    if (STR_ASSOC_P(str)) {
	FL_UNSET(str, STR_ASSOC);
	capa = RSTRING(str)->as.heap.aux.capa = RSTRING_LEN(str);
    }
    else if (STR_EMBED_P(str)) {
	capa = RSTRING_EMBED_LEN_MAX;
    }
    else {
	capa = RSTRING(str)->as.heap.aux.capa;
    }
    if (RSTRING_LEN(str) >= LONG_MAX - len) {
	rb_raise(rb_eArgError, "string sizes too big");
    }
    total = RSTRING_LEN(str)+len;
    if (capa <= total) {
	while (total > capa) {
	    if (capa + 1 >= LONG_MAX / 2) {
		capa = (total + 4095) / 4096;
		break;
	    }
	    capa = (capa + 1) * 2;
	}
	RESIZE_CAPA(str, capa);
    }
    if (off != -1) {
        ptr = RSTRING_PTR(str) + off;
    }
    memcpy(RSTRING_PTR(str) + RSTRING_LEN(str), ptr, len);
    STR_SET_LEN(str, total);
    RSTRING_PTR(str)[total] = '\0'; /* sentinel */

    return str;
}