static int decode_commit(struct xdr_stream *xdr, struct nfs_commitres *res)
{
	struct nfs_writeverf *verf = res->verf;
	int status;

	status = decode_op_hdr(xdr, OP_COMMIT);
	if (!status)
		status = decode_write_verifier(xdr, &verf->verifier);
	if (!status)
		verf->committed = NFS_FILE_SYNC;
	return status;
}