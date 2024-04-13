static int nfs4_do_set_security_label(struct inode *inode,
		struct nfs4_label *ilabel,
		struct nfs_fattr *fattr,
		struct nfs4_label *olabel)
{
	struct nfs4_exception exception = { };
	int err;

	do {
		err = _nfs4_do_set_security_label(inode, ilabel,
				fattr, olabel);
		trace_nfs4_set_security_label(inode, err);
		err = nfs4_handle_exception(NFS_SERVER(inode), err,
				&exception);
	} while (exception.retry);
	return err;
}