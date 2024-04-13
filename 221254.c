str_discard(VALUE str)
{
    str_modifiable(str);
    if (!STR_SHARED_P(str) && !STR_EMBED_P(str)) {
	xfree(RSTRING_PTR(str));
	RSTRING(str)->as.heap.ptr = 0;
	RSTRING(str)->as.heap.len = 0;
    }
}