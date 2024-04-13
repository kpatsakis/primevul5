static void encode_lockowner(struct xdr_stream *xdr, const struct nfs_lowner *lowner)
{
	__be32 *p;

	p = reserve_space(xdr, 32);
	p = xdr_encode_hyper(p, lowner->clientid);
	*p++ = cpu_to_be32(20);
	p = xdr_encode_opaque_fixed(p, "lock id:", 8);
	*p++ = cpu_to_be32(lowner->s_dev);
	xdr_encode_hyper(p, lowner->id);
}