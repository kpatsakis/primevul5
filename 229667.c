static void encode_bind_conn_to_session(struct xdr_stream *xdr,
				   const struct nfs41_bind_conn_to_session_args *args,
				   struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_BIND_CONN_TO_SESSION,
		decode_bind_conn_to_session_maxsz, hdr);
	encode_opaque_fixed(xdr, args->sessionid.data, NFS4_MAX_SESSIONID_LEN);
	p = xdr_reserve_space(xdr, 8);
	*p++ = cpu_to_be32(args->dir);
	*p = (args->use_conn_in_rdma_mode) ? cpu_to_be32(1) : cpu_to_be32(0);
}