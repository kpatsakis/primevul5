static int nfs4_xdr_dec_release_lockowner(struct rpc_rqst *rqstp,
					  struct xdr_stream *xdr, void *dummy)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_release_lockowner(xdr);
	return status;
}