static inline void encode_claim_fh(struct xdr_stream *xdr)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(NFS4_OPEN_CLAIM_FH);
}