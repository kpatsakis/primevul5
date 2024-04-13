int fuse_update_attributes(struct inode *inode, struct file *file)
{
	/* Do *not* need to get atime for internal purposes */
	return fuse_update_get_attr(inode, file, NULL,
				    STATX_BASIC_STATS & ~STATX_ATIME, 0);
}