static void encode_commit(struct xdr_stream *xdr, const struct nfs_commitargs *args, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_COMMIT, decode_commit_maxsz, hdr);
	p = reserve_space(xdr, 12);
	p = xdr_encode_hyper(p, args->offset);
	*p = cpu_to_be32(args->count);
}