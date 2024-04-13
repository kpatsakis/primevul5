static int decode_layout_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	stateid->type = NFS4_LAYOUT_STATEID_TYPE;
	return decode_stateid(xdr, stateid);
}