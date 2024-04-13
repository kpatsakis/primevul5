encode_putfh(struct xdr_stream *xdr, const struct nfs_fh *fh, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_PUTFH, decode_putfh_maxsz, hdr);
	encode_string(xdr, fh->size, fh->data);
}