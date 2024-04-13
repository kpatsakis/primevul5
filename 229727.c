static void nfs4_xdr_enc_statfs(struct rpc_rqst *req, struct xdr_stream *xdr,
				const void *data)
{
	const struct nfs4_statfs_arg *args = data;
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_getattr(xdr, nfs4_statfs_bitmap, args->bitmask,
			ARRAY_SIZE(nfs4_statfs_bitmap), &hdr);
	encode_nops(&hdr);
}