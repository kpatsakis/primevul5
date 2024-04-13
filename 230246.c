static int nfs4_xdr_dec_getdeviceinfo(struct rpc_rqst *rqstp,
				      struct xdr_stream *xdr,
				      void *data)
{
	struct nfs4_getdeviceinfo_res *res = data;
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (status != 0)
		goto out;
	status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (status != 0)
		goto out;
	status = decode_getdeviceinfo(xdr, res);
out:
	return status;
}