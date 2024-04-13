rb_str_capacity(VALUE str)
{
    if (STR_EMBED_P(str)) {
	return RSTRING_EMBED_LEN_MAX;
    }
    else if (STR_NOCAPA_P(str)) {
	return RSTRING(str)->as.heap.len;
    }
    else {
	return RSTRING(str)->as.heap.aux.capa;
    }
}