static void encode_opaque_fixed(struct xdr_stream *xdr, const void *buf, size_t len)
{
	WARN_ON_ONCE(xdr_stream_encode_opaque_fixed(xdr, buf, len) < 0);
}