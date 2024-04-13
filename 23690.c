static int fuse_fsync(struct file *file, struct dentry *de, int datasync)
{
	return fuse_fsync_common(file, de, datasync, 0);
}