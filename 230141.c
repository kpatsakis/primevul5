static int decode_close(struct xdr_stream *xdr, struct nfs_closeres *res)
{
	int status;

	status = decode_op_hdr(xdr, OP_CLOSE);
	if (status != -EIO)
		nfs_increment_open_seqid(status, res->seqid);
	if (!status)
		status = decode_invalid_stateid(xdr, &res->stateid);
	return status;
}