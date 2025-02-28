static int decode_layoutcommit(struct xdr_stream *xdr,
			       struct rpc_rqst *req,
			       struct nfs4_layoutcommit_res *res)
{
	__be32 *p;
	__u32 sizechanged;
	int status;

	status = decode_op_hdr(xdr, OP_LAYOUTCOMMIT);
	res->status = status;
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	sizechanged = be32_to_cpup(p);

	if (sizechanged) {
		/* throw away new size */
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			return -EIO;
	}
	return 0;
}