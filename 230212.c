static int decode_delegreturn(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_DELEGRETURN);
}