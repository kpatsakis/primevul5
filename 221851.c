static int no_xattr_get(const struct xattr_handler *handler,
			struct dentry *dentry, struct inode *inode,
			const char *name, void *value, size_t size)
{
	return -EOPNOTSUPP;
}