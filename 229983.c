static int nfs4_xdr_dec_renew(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			      void *__unused)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_renew(xdr);
	return status;
}