static int decode_putfh(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_PUTFH);
}