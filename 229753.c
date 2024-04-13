static void nfs4_xdr_enc_getdeviceinfo(struct rpc_rqst *req,
				       struct xdr_stream *xdr,
				       const void *data)
{
	const struct nfs4_getdeviceinfo_args *args = data;
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_getdeviceinfo(xdr, args, &hdr);

	/* set up reply kvec. Subtract notification bitmap max size (2)
	 * so that notification bitmap is put in xdr_buf tail */
	rpc_prepare_reply_pages(req, args->pdev->pages, args->pdev->pgbase,
				args->pdev->pglen, hdr.replen - 2);
	encode_nops(&hdr);
}