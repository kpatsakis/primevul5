decode_savefh(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_SAVEFH);
}