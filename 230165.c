static int nfs4_xdr_dec_destroy_clientid(struct rpc_rqst *rqstp,
					struct xdr_stream *xdr,
					void *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_destroy_clientid(xdr, res);
	return status;
}