encode_savefh(struct xdr_stream *xdr, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_SAVEFH, decode_savefh_maxsz, hdr);
}