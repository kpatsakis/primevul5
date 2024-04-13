static void nfs4_xdr_enc_open_noattr(struct rpc_rqst *req,
				     struct xdr_stream *xdr,
				     const void *data)
{
	const struct nfs_openargs *args = data;
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_open(xdr, args, &hdr);
	if (args->access)
		encode_access(xdr, args->access, &hdr);
	encode_getfattr_open(xdr, args->bitmask, args->open_bitmap, &hdr);
	if (args->lg_args) {
		encode_layoutget(xdr, args->lg_args, &hdr);
		rpc_prepare_reply_pages(req, args->lg_args->layout.pages, 0,
					args->lg_args->layout.pglen,
					hdr.replen);
	}
	encode_nops(&hdr);
}