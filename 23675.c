static ssize_t fuse_direct_read(struct file *file, char __user *buf,
				     size_t count, loff_t *ppos)
{
	ssize_t res;
	struct inode *inode = file->f_path.dentry->d_inode;

	if (is_bad_inode(inode))
		return -EIO;

	res = fuse_direct_io(file, buf, count, ppos, 0);

	fuse_invalidate_attr(inode);

	return res;
}