static void encode_remove(struct xdr_stream *xdr, const struct qstr *name, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_REMOVE, decode_remove_maxsz, hdr);
	encode_string(xdr, name->len, name->name);
}