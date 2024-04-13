static int decode_write_verifier(struct xdr_stream *xdr, struct nfs_write_verifier *verifier)
{
	return decode_opaque_fixed(xdr, verifier->data, NFS4_VERIFIER_SIZE);
}