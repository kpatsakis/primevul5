static void encode_getfh(struct xdr_stream *xdr, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_GETFH, decode_getfh_maxsz, hdr);
}