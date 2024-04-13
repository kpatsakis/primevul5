encode_layoutget(struct xdr_stream *xdr,
		      const struct nfs4_layoutget_args *args,
		      struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_LAYOUTGET, decode_layoutget_maxsz, hdr);
	p = reserve_space(xdr, 36);
	*p++ = cpu_to_be32(0);     /* Signal layout available */
	*p++ = cpu_to_be32(args->type);
	*p++ = cpu_to_be32(args->range.iomode);
	p = xdr_encode_hyper(p, args->range.offset);
	p = xdr_encode_hyper(p, args->range.length);
	p = xdr_encode_hyper(p, args->minlength);
	encode_nfs4_stateid(xdr, &args->stateid);
	encode_uint32(xdr, args->maxcount);

	dprintk("%s: 1st type:0x%x iomode:%d off:%lu len:%lu mc:%d\n",
		__func__,
		args->type,
		args->range.iomode,
		(unsigned long)args->range.offset,
		(unsigned long)args->range.length,
		args->maxcount);
}