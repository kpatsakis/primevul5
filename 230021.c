static int decode_readdir(struct xdr_stream *xdr, struct rpc_rqst *req, struct nfs4_readdir_res *readdir)
{
	int		status;
	__be32		verf[2];

	status = decode_op_hdr(xdr, OP_READDIR);
	if (!status)
		status = decode_verifier(xdr, readdir->verifier.data);
	if (unlikely(status))
		return status;
	memcpy(verf, readdir->verifier.data, sizeof(verf));
	dprintk("%s: verifier = %08x:%08x\n",
			__func__, verf[0], verf[1]);
	return xdr_read_pages(xdr, xdr->buf->page_len);
}