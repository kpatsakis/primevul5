static int decode_release_lockowner(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_RELEASE_LOCKOWNER);
}