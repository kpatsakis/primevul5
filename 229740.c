static int nfs4_proc_lookupp(struct inode *inode, struct nfs_fh *fhandle,
			     struct nfs_fattr *fattr, struct nfs4_label *label)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;
	do {
		err = _nfs4_proc_lookupp(inode, fhandle, fattr, label);
		trace_nfs4_lookupp(inode, err);
		err = nfs4_handle_exception(NFS_SERVER(inode), err,
				&exception);
	} while (exception.retry);
	return err;
}