static void encode_write(struct xdr_stream *xdr, const struct nfs_pgio_args *args,
			 struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_WRITE, decode_write_maxsz, hdr);
	encode_nfs4_stateid(xdr, &args->stateid);

	p = reserve_space(xdr, 16);
	p = xdr_encode_hyper(p, args->offset);
	*p++ = cpu_to_be32(args->stable);
	*p = cpu_to_be32(args->count);

	xdr_write_pages(xdr, args->pages, args->pgbase, args->count);
}