static int nfs4_xdr_dec_setclientid_confirm(struct rpc_rqst *req,
					    struct xdr_stream *xdr,
					    void *data)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_setclientid_confirm(xdr);
	return status;
}