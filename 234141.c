static ssize_t random_write(struct file *file, const char __user *buffer,
			    size_t count, loff_t *ppos)
{
	size_t ret;
	struct inode *inode = file->f_path.dentry->d_inode;

	ret = write_pool(&blocking_pool, buffer, count);
	if (ret)
		return ret;
	ret = write_pool(&nonblocking_pool, buffer, count);
	if (ret)
		return ret;

	inode->i_mtime = current_fs_time(inode->i_sb);
	mark_inode_dirty(inode);
	return (ssize_t)count;
}