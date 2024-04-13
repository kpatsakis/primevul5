int nfs4_proc_commit(struct file *dst, __u64 offset, __u32 count, struct nfs_commitres *res)
{
	struct nfs_commitargs args = {
		.offset = offset,
		.count = count,
	};
	struct nfs_server *dst_server = NFS_SERVER(file_inode(dst));
	struct nfs4_exception exception = { };
	int status;

	do {
		status = _nfs4_proc_commit(dst, &args, res);
		status = nfs4_handle_exception(dst_server, status, &exception);
	} while (exception.retry);

	return status;
}