encode_layoutreturn(struct xdr_stream *xdr,
		    const struct nfs4_layoutreturn_args *args,
		    struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_LAYOUTRETURN, decode_layoutreturn_maxsz, hdr);
	p = reserve_space(xdr, 16);
	*p++ = cpu_to_be32(0);		/* reclaim. always 0 for now */
	*p++ = cpu_to_be32(args->layout_type);
	*p++ = cpu_to_be32(args->range.iomode);
	*p = cpu_to_be32(RETURN_FILE);
	p = reserve_space(xdr, 16);
	p = xdr_encode_hyper(p, args->range.offset);
	p = xdr_encode_hyper(p, args->range.length);
	spin_lock(&args->inode->i_lock);
	encode_nfs4_stateid(xdr, &args->stateid);
	spin_unlock(&args->inode->i_lock);
	if (args->ld_private->ops && args->ld_private->ops->encode)
		args->ld_private->ops->encode(xdr, args, args->ld_private);
	else
		encode_uint32(xdr, 0);
}