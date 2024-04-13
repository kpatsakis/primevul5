static void encode_readlink(struct xdr_stream *xdr, const struct nfs4_readlink *readlink, struct rpc_rqst *req, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_READLINK, decode_readlink_maxsz, hdr);
}