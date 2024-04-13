static int decode_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	return decode_opaque_fixed(xdr, stateid, NFS4_STATEID_SIZE);
}