static int decode_reclaim_complete(struct xdr_stream *xdr, void *dummy)
{
	return decode_op_hdr(xdr, OP_RECLAIM_COMPLETE);
}