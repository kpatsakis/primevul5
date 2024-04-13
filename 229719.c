static void encode_op_hdr(struct xdr_stream *xdr, enum nfs_opnum4 op,
		uint32_t replen,
		struct compound_hdr *hdr)
{
	encode_uint32(xdr, op);
	hdr->nops++;
	hdr->replen += replen;
}