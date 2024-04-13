static int decode_rename(struct xdr_stream *xdr, struct nfs4_change_info *old_cinfo,
	      struct nfs4_change_info *new_cinfo)
{
	int status;

	status = decode_op_hdr(xdr, OP_RENAME);
	if (status)
		goto out;
	if ((status = decode_change_info(xdr, old_cinfo)))
		goto out;
	status = decode_change_info(xdr, new_cinfo);
out:
	return status;
}