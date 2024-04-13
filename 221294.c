str_make_independent(VALUE str)
{
    char *ptr;
    long len = RSTRING_LEN(str);

    ptr = ALLOC_N(char, len+1);
    if (RSTRING_PTR(str)) {
	memcpy(ptr, RSTRING_PTR(str), len);
    }
    STR_SET_NOEMBED(str);
    ptr[len] = 0;
    RSTRING(str)->as.heap.ptr = ptr;
    RSTRING(str)->as.heap.len = len;
    RSTRING(str)->as.heap.aux.capa = len;
    STR_UNSET_NOCAPA(str);
}