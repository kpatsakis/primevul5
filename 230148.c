static int decode_lockt(struct xdr_stream *xdr, struct nfs_lockt_res *res)
{
	int status;
	status = decode_op_hdr(xdr, OP_LOCKT);
	if (status == -NFS4ERR_DENIED)
		return decode_lock_denied(xdr, res->denied);
	return status;
}