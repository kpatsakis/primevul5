static int decode_lookupp(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_LOOKUPP);
}