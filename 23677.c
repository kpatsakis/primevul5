static int fuse_release(struct inode *inode, struct file *file)
{
	fuse_release_common(file, FUSE_RELEASE);

	/* return value is ignored by VFS */
	return 0;
}