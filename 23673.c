static long fuse_file_ioctl_common(struct file *file, unsigned int cmd,
				   unsigned long arg, unsigned int flags)
{
	struct inode *inode = file->f_dentry->d_inode;
	struct fuse_conn *fc = get_fuse_conn(inode);

	if (!fuse_allow_task(fc, current))
		return -EACCES;

	if (is_bad_inode(inode))
		return -EIO;

	return fuse_do_ioctl(file, cmd, arg, flags);
}