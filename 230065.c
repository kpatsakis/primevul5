static int nfs4_get_security_label(struct inode *inode, void *buf,
					size_t buflen)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;

	if (!nfs_server_capable(inode, NFS_CAP_SECURITY_LABEL))
		return -EOPNOTSUPP;

	do {
		err = _nfs4_get_security_label(inode, buf, buflen);
		trace_nfs4_get_security_label(inode, err);
		err = nfs4_handle_exception(NFS_SERVER(inode), err,
				&exception);
	} while (exception.retry);
	return err;
}