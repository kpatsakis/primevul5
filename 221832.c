static int fuse_dir_release(struct inode *inode, struct file *file)
{
	fuse_release_common(file, true);

	return 0;
}