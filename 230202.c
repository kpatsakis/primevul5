static int decode_open_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	stateid->type = NFS4_OPEN_STATEID_TYPE;
	return decode_stateid(xdr, stateid);
}