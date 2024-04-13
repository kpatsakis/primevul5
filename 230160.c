static void encode_destroy_clientid(struct xdr_stream *xdr,
				   uint64_t clientid,
				   struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_DESTROY_CLIENTID, decode_destroy_clientid_maxsz, hdr);
	encode_uint64(xdr, clientid);
}