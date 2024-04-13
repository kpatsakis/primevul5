static int nfs4_proc_remove(struct inode *dir, struct dentry *dentry)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	struct inode *inode = d_inode(dentry);
	int err;

	if (inode) {
		if (inode->i_nlink == 1)
			nfs4_inode_return_delegation(inode);
		else
			nfs4_inode_make_writeable(inode);
	}
	do {
		err = _nfs4_proc_remove(dir, &dentry->d_name, NF4REG);
		trace_nfs4_remove(dir, &dentry->d_name, err);
		err = nfs4_handle_exception(NFS_SERVER(dir), err,
				&exception);
	} while (exception.retry);
	return err;
}