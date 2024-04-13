rb_str_free(VALUE str)
{
    if (!STR_EMBED_P(str) && !STR_SHARED_P(str)) {
	xfree(RSTRING(str)->as.heap.ptr);
    }
}