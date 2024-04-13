static void encode_lookupp(struct xdr_stream *xdr, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_LOOKUPP, decode_lookupp_maxsz, hdr);
}