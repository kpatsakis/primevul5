static int nfs4_proc_link(struct inode *inode, struct inode *dir, const struct qstr *name)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(inode),
				_nfs4_proc_link(inode, dir, name),
				&exception);
	} while (exception.retry);
	return err;
}