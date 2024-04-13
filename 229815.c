static inline void encode_claim_delegate_cur(struct xdr_stream *xdr, const struct qstr *name, const nfs4_stateid *stateid)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(NFS4_OPEN_CLAIM_DELEGATE_CUR);
	encode_nfs4_stateid(xdr, stateid);
	encode_string(xdr, name->len, name->name);
}