static int nfs4_xattr_get_nfs4_label(const struct xattr_handler *handler,
				     struct dentry *unused, struct inode *inode,
				     const char *key, void *buf, size_t buflen)
{
	if (security_ismaclabel(key))
		return nfs4_get_security_label(inode, buf, buflen);
	return -EOPNOTSUPP;
}