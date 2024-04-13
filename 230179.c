static int decode_delegation_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	stateid->type = NFS4_DELEGATION_STATEID_TYPE;
	return decode_stateid(xdr, stateid);
}