str_new(VALUE klass, const char *ptr, long len)
{
    VALUE str;

    if (len < 0) {
	rb_raise(rb_eArgError, "negative string size (or size too big)");
    }

    str = str_alloc(klass);
    if (len > RSTRING_EMBED_LEN_MAX) {
	RSTRING(str)->as.heap.aux.capa = len;
	RSTRING(str)->as.heap.ptr = ALLOC_N(char,len+1);
	STR_SET_NOEMBED(str);
    }
    else if (len == 0) {
	ENC_CODERANGE_SET(str, ENC_CODERANGE_7BIT);
    }
    if (ptr) {
	memcpy(RSTRING_PTR(str), ptr, len);
    }
    STR_SET_LEN(str, len);
    RSTRING_PTR(str)[len] = '\0';
    return str;
}