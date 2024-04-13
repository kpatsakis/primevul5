static int nfs4_xdr_dec_lookup_root(struct rpc_rqst *rqstp,
				    struct xdr_stream *xdr,
				    void *data)
{
	struct nfs4_lookup_res *res = data;
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (status)
		goto out;
	status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (status)
		goto out;
	status = decode_putrootfh(xdr);
	if (status)
		goto out;
	status = decode_getfh(xdr, res->fh);
	if (status == 0)
		status = decode_getfattr_label(xdr, res->fattr,
						res->label, res->server);
out:
	return status;
}