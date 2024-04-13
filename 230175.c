static int decode_setclientid_confirm(struct xdr_stream *xdr)
{
	return decode_op_hdr(xdr, OP_SETCLIENTID_CONFIRM);
}