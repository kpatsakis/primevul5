ssize_t xdr_stream_decode_string_dup(struct xdr_stream *xdr, char **str,
		size_t maxlen, gfp_t gfp_flags)
{
	void *p;
	ssize_t ret;

	ret = xdr_stream_decode_opaque_inline(xdr, &p, maxlen);
	if (ret > 0) {
		char *s = kmemdup_nul(p, ret, gfp_flags);
		if (s != NULL) {
			*str = s;
			return strlen(s);
		}
		ret = -ENOMEM;
	}
	*str = NULL;
	return ret;
}