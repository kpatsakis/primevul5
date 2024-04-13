encode_restorefh(struct xdr_stream *xdr, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_RESTOREFH, decode_restorefh_maxsz, hdr);
}