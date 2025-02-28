static int nfs4_xdr_dec_create_session(struct rpc_rqst *rqstp,
				       struct xdr_stream *xdr,
				       void *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_create_session(xdr, res);
	return status;
}