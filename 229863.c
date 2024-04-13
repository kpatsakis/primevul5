static void encode_nfs4_stateid(struct xdr_stream *xdr, const nfs4_stateid *stateid)
{
	encode_opaque_fixed(xdr, stateid, NFS4_STATEID_SIZE);
}