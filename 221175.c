rb_str_memsize(VALUE str)
{
    if (!STR_EMBED_P(str) && !STR_SHARED_P(str)) {
	return RSTRING(str)->as.heap.aux.capa;
    }
    else {
	return 0;
    }
}