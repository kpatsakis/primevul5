static void encode_uint32(struct xdr_stream *xdr, u32 n)
{
	WARN_ON_ONCE(xdr_stream_encode_u32(xdr, n) < 0);
}