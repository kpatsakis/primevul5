static int fuse_open(struct inode *inode, struct file *file)
{
	return fuse_open_common(inode, file, false);
}