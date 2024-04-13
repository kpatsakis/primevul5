static int fuse_xattr_get(const struct xattr_handler *handler,
			 struct dentry *dentry, struct inode *inode,
			 const char *name, void *value, size_t size)
{
	if (fuse_is_bad(inode))
		return -EIO;

	return fuse_getxattr(inode, name, value, size);
}