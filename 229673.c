static void encode_open_confirm(struct xdr_stream *xdr, const struct nfs_open_confirmargs *arg, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_OPEN_CONFIRM, decode_open_confirm_maxsz, hdr);
	encode_nfs4_stateid(xdr, arg->stateid);
	encode_nfs4_seqid(xdr, arg->seqid);
}