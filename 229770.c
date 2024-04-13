static int decode_lock_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	stateid->type = NFS4_LOCK_STATEID_TYPE;
	return decode_stateid(xdr, stateid);
}