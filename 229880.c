static int nfs4_xattr_set_nfs4_acl(const struct xattr_handler *handler,
				   struct dentry *unused, struct inode *inode,
				   const char *key, const void *buf,
				   size_t buflen, int flags)
{
	return nfs4_proc_set_acl(inode, buf, buflen);
}