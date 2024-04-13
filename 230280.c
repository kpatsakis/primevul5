static int decode_destroy_session(struct xdr_stream *xdr, void *dummy)
{
	return decode_op_hdr(xdr, OP_DESTROY_SESSION);
}