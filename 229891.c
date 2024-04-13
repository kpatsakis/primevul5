static int decode_putrootfh(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_PUTROOTFH);
}