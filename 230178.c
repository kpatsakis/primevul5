static int decode_opaque_fixed(struct xdr_stream *xdr, void *buf, size_t len)
{
	ssize_t ret = xdr_stream_decode_opaque_fixed(xdr, buf, len);
	if (unlikely(ret < 0))
		return -EIO;
	return 0;
}