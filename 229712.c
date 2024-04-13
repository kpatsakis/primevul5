static void encode_share_access(struct xdr_stream *xdr, u32 share_access)
{
	__be32 *p;

	p = reserve_space(xdr, 8);
	*p++ = cpu_to_be32(share_access);
	*p = cpu_to_be32(0);		/* for linux, share_deny = 0 always */
}