static void encode_link(struct xdr_stream *xdr, const struct qstr *name, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_LINK, decode_link_maxsz, hdr);
	encode_string(xdr, name->len, name->name);
}