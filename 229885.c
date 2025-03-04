static int nfs4_proc_mknod(struct inode *dir, struct dentry *dentry,
		struct iattr *sattr, dev_t rdev)
{
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	struct nfs4_label l, *label = NULL;
	int err;

	label = nfs4_label_init_security(dir, dentry, sattr, &l);

	if (!(server->attr_bitmask[2] & FATTR4_WORD2_MODE_UMASK))
		sattr->ia_mode &= ~current_umask();
	do {
		err = _nfs4_proc_mknod(dir, dentry, sattr, label, rdev);
		trace_nfs4_mknod(dir, &dentry->d_name, err);
		err = nfs4_handle_exception(NFS_SERVER(dir), err,
				&exception);
	} while (exception.retry);

	nfs4_label_release_security(label);

	return err;
}