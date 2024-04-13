static inline void encode_claim_null(struct xdr_stream *xdr, const struct qstr *name)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(NFS4_OPEN_CLAIM_NULL);
	encode_string(xdr, name->len, name->name);
}