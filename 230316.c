static int decode_verifier(struct xdr_stream *xdr, void *verifier)
{
	return decode_opaque_fixed(xdr, verifier, NFS4_VERIFIER_SIZE);
}