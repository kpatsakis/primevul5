static void encode_secinfo(struct xdr_stream *xdr, const struct qstr *name, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_SECINFO, decode_secinfo_maxsz, hdr);
	encode_string(xdr, name->len, name->name);
}