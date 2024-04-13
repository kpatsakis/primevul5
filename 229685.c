static void encode_uint64(struct xdr_stream *xdr, u64 n)
{
	WARN_ON_ONCE(xdr_stream_encode_u64(xdr, n) < 0);
}