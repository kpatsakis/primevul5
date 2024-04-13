static inline void encode_claim_delegate_cur_fh(struct xdr_stream *xdr, const nfs4_stateid *stateid)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(NFS4_OPEN_CLAIM_DELEG_CUR_FH);
	encode_nfs4_stateid(xdr, stateid);
}