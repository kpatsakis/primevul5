static int decode_renew(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_RENEW);
}