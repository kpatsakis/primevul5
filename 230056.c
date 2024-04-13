static int decode_secinfo(struct xdr_stream *xdr, struct nfs4_secinfo_res *res)
{
	int status = decode_op_hdr(xdr, OP_SECINFO);
	if (status)
		return status;
	return decode_secinfo_common(xdr, res);
}