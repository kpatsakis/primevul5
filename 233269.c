ssize_t xdr_stream_decode_opaque_dup(struct xdr_stream *xdr, void **ptr,
		size_t maxlen, gfp_t gfp_flags)
{
	ssize_t ret;
	void *p;

	ret = xdr_stream_decode_opaque_inline(xdr, &p, maxlen);
	if (ret > 0) {
		*ptr = kmemdup(p, ret, gfp_flags);
		if (*ptr != NULL)
			return ret;
		ret = -ENOMEM;
	}
	*ptr = NULL;
	return ret;
}