static void encode_nfs4_verifier(struct xdr_stream *xdr, const nfs4_verifier *verf)
{
	encode_opaque_fixed(xdr, verf->data, NFS4_VERIFIER_SIZE);
}