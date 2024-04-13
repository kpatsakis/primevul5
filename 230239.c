static void encode_string(struct xdr_stream *xdr, unsigned int len, const char *str)
{
	WARN_ON_ONCE(xdr_stream_encode_opaque(xdr, str, len) < 0);
}