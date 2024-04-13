rb_str_resize(VALUE str, long len)
{
    long slen;

    if (len < 0) {
	rb_raise(rb_eArgError, "negative string size (or size too big)");
    }

    rb_str_modify(str);
    slen = RSTRING_LEN(str);
    if (len != slen) {
	if (STR_EMBED_P(str)) {
	    char *ptr;
	    if (len <= RSTRING_EMBED_LEN_MAX) {
		STR_SET_EMBED_LEN(str, len);
		RSTRING(str)->as.ary[len] = '\0';
		return str;
	    }
	    ptr = ALLOC_N(char,len+1);
	    MEMCPY(ptr, RSTRING(str)->as.ary, char, slen);
	    RSTRING(str)->as.heap.ptr = ptr;
	    STR_SET_NOEMBED(str);
	}
	else if (len <= RSTRING_EMBED_LEN_MAX) {
	    char *ptr = RSTRING(str)->as.heap.ptr;
	    STR_SET_EMBED(str);
	    if (slen > 0) MEMCPY(RSTRING(str)->as.ary, ptr, char, len);
	    RSTRING(str)->as.ary[len] = '\0';
	    STR_SET_EMBED_LEN(str, len);
	    xfree(ptr);
	    return str;
	}
	else if (slen < len || slen - len > 1024) {
	    REALLOC_N(RSTRING(str)->as.heap.ptr, char, len+1);
	}
	if (!STR_NOCAPA_P(str)) {
	    RSTRING(str)->as.heap.aux.capa = len;
	}
	RSTRING(str)->as.heap.len = len;
	RSTRING(str)->as.heap.ptr[len] = '\0';	/* sentinel */
    }
    return str;
}