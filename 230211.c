static int decode_lookup(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_LOOKUP);
}