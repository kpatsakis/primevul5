static void encode_access(struct xdr_stream *xdr, u32 access, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_ACCESS, decode_access_maxsz, hdr);
	encode_uint32(xdr, access);
}