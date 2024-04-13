static void nfs4_xdr_enc_layoutget(struct rpc_rqst *req,
				   struct xdr_stream *xdr,
				   const void *data)
{
	const struct nfs4_layoutget_args *args = data;
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, NFS_FH(args->inode), &hdr);
	encode_layoutget(xdr, args, &hdr);

	rpc_prepare_reply_pages(req, args->layout.pages, 0,
				args->layout.pglen, hdr.replen);
	encode_nops(&hdr);
}