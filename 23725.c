static ssize_t fuse_direct_write(struct file *file, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	ssize_t res;

	if (is_bad_inode(inode))
		return -EIO;

	/* Don't allow parallel writes to the same file */
	mutex_lock(&inode->i_mutex);
	res = generic_write_checks(file, ppos, &count, 0);
	if (!res) {
		res = fuse_direct_io(file, buf, count, ppos, 1);
		if (res > 0)
			fuse_write_update_size(inode, *ppos);
	}
	mutex_unlock(&inode->i_mutex);

	fuse_invalidate_attr(inode);

	return res;
}