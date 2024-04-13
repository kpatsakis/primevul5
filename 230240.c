static void encode_renew(struct xdr_stream *xdr, clientid4 clid,
			 struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_RENEW, decode_renew_maxsz, hdr);
	encode_uint64(xdr, clid);
}