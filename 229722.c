static int decode_link(struct xdr_stream *xdr, struct nfs4_change_info *cinfo)
{
	int status;

	status = decode_op_hdr(xdr, OP_LINK);
	if (status)
		return status;
	return decode_change_info(xdr, cinfo);
}