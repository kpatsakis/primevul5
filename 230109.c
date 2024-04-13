static void nfs4_xdr_enc_bind_conn_to_session(struct rpc_rqst *req,
				struct xdr_stream *xdr,
				const void *data)
{
	const struct nfs41_bind_conn_to_session_args *args = data;
	struct compound_hdr hdr = {
		.minorversion = args->client->cl_mvops->minor_version,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_bind_conn_to_session(xdr, args, &hdr);
	encode_nops(&hdr);
}