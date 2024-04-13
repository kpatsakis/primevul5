rb_str_drop_bytes(VALUE str, long len)
{
    char *ptr = RSTRING_PTR(str);
    long olen = RSTRING_LEN(str), nlen;

    str_modifiable(str);
    if (len > olen) len = olen;
    nlen = olen - len;
    if (nlen <= RSTRING_EMBED_LEN_MAX) {
	char *oldptr = ptr;
	int fl = (int)(RBASIC(str)->flags & (STR_NOEMBED|ELTS_SHARED));
	STR_SET_EMBED(str);
	STR_SET_EMBED_LEN(str, nlen);
	ptr = RSTRING(str)->as.ary;
	memmove(ptr, oldptr + len, nlen);
	if (fl == STR_NOEMBED) xfree(oldptr);
    }
    else {
	if (!STR_SHARED_P(str)) rb_str_new4(str);
	ptr = RSTRING(str)->as.heap.ptr += len;
	RSTRING(str)->as.heap.len = nlen;
    }
    ptr[nlen] = 0;
    ENC_CODERANGE_CLEAR(str);
    return str;
}