static void encode_putrootfh(struct xdr_stream *xdr, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_PUTROOTFH, decode_putrootfh_maxsz, hdr);
}