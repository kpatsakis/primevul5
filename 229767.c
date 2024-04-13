static void encode_read(struct xdr_stream *xdr, const struct nfs_pgio_args *args,
			struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_READ, decode_read_maxsz, hdr);
	encode_nfs4_stateid(xdr, &args->stateid);

	p = reserve_space(xdr, 12);
	p = xdr_encode_hyper(p, args->offset);
	*p = cpu_to_be32(args->count);
}