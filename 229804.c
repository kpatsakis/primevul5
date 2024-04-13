static int nfs4_xdr_dec_symlink(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
				void *res)
{
	return nfs4_xdr_dec_create(rqstp, xdr, res);
}