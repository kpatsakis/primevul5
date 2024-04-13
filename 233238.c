ssize_t xdr_stream_decode_opaque(struct xdr_stream *xdr, void *ptr, size_t size)
{
	ssize_t ret;
	void *p;

	ret = xdr_stream_decode_opaque_inline(xdr, &p, size);
	if (ret <= 0)
		return ret;
	memcpy(ptr, p, ret);
	return ret;
}