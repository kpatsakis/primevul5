static int nfs4_xattr_get_nfs4_acl(const struct xattr_handler *handler,
				   struct dentry *unused, struct inode *inode,
				   const char *key, void *buf, size_t buflen)
{
	return nfs4_proc_get_acl(inode, buf, buflen);
}