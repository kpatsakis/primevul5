static int nfs4_proc_rmdir(struct inode *dir, const struct qstr *name)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;

	do {
		err = _nfs4_proc_remove(dir, name, NF4DIR);
		trace_nfs4_remove(dir, name, err);
		err = nfs4_handle_exception(NFS_SERVER(dir), err,
				&exception);
	} while (exception.retry);
	return err;
}