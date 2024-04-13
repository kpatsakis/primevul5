static void encode_lookup(struct xdr_stream *xdr, const struct qstr *name, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_LOOKUP, decode_lookup_maxsz, hdr);
	encode_string(xdr, name->len, name->name);
}