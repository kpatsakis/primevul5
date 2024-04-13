rb_str_buf_new(long capa)
{
    VALUE str = str_alloc(rb_cString);

    if (capa < STR_BUF_MIN_SIZE) {
	capa = STR_BUF_MIN_SIZE;
    }
    FL_SET(str, STR_NOEMBED);
    RSTRING(str)->as.heap.aux.capa = capa;
    RSTRING(str)->as.heap.ptr = ALLOC_N(char, capa+1);
    RSTRING(str)->as.heap.ptr[0] = '\0';

    return str;
}