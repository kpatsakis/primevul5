static int decode_getfattr_label(struct xdr_stream *xdr, struct nfs_fattr *fattr,
		struct nfs4_label *label, const struct nfs_server *server)
{
	return decode_getfattr_generic(xdr, fattr, NULL, NULL, label, server);
}