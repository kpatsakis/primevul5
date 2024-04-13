static int nfs4_xattr_set_nfs4_label(const struct xattr_handler *handler,
				     struct dentry *unused, struct inode *inode,
				     const char *key, const void *buf,
				     size_t buflen, int flags)
{
	if (security_ismaclabel(key))
		return nfs4_set_security_label(inode, buf, buflen);

	return -EOPNOTSUPP;
}