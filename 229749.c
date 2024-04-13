static int decode_change_info(struct xdr_stream *xdr, struct nfs4_change_info *cinfo)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, 20);
	if (unlikely(!p))
		return -EIO;
	cinfo->atomic = be32_to_cpup(p++);
	p = xdr_decode_hyper(p, &cinfo->before);
	xdr_decode_hyper(p, &cinfo->after);
	return 0;
}