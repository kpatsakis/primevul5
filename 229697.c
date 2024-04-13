static void encode_release_lockowner(struct xdr_stream *xdr, const struct nfs_lowner *lowner, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_RELEASE_LOCKOWNER, decode_release_lockowner_maxsz, hdr);
	encode_lockowner(xdr, lowner);
}