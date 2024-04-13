static int fuse_xattr_set(const struct xattr_handler *handler,
			  struct dentry *dentry, struct inode *inode,
			  const char *name, const void *value, size_t size,
			  int flags)
{
	if (fuse_is_bad(inode))
		return -EIO;

	if (!value)
		return fuse_removexattr(inode, name);

	return fuse_setxattr(inode, name, value, size, flags);
}