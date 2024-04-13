static int no_xattr_set(const struct xattr_handler *handler,
			struct dentry *dentry, struct inode *nodee,
			const char *name, const void *value,
			size_t size, int flags)
{
	return -EOPNOTSUPP;
}