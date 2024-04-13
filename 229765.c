static int decode_invalid_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	nfs4_stateid dummy;

	nfs4_stateid_copy(stateid, &invalid_stateid);
	return decode_stateid(xdr, &dummy);
}