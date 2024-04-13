static int nfs4_proc_readdir(struct dentry *dentry, const struct cred *cred,
		u64 cookie, struct page **pages, unsigned int count, bool plus)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;
	do {
		err = _nfs4_proc_readdir(dentry, cred, cookie,
				pages, count, plus);
		trace_nfs4_readdir(d_inode(dentry), err);
		err = nfs4_handle_exception(NFS_SERVER(d_inode(dentry)), err,
				&exception);
	} while (exception.retry);
	return err;
}