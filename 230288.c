static void encode_delegreturn(struct xdr_stream *xdr, const nfs4_stateid *stateid, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_DELEGRETURN, decode_delegreturn_maxsz, hdr);
	encode_nfs4_stateid(xdr, stateid);
}