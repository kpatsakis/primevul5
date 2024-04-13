static ssize_t fuse_file_aio_write(struct kiocb *iocb, const struct iovec *iov,
				   unsigned long nr_segs, loff_t pos)
{
	struct file *file = iocb->ki_filp;
	struct address_space *mapping = file->f_mapping;
	size_t count = 0;
	ssize_t written = 0;
	struct inode *inode = mapping->host;
	ssize_t err;
	struct iov_iter i;

	WARN_ON(iocb->ki_pos != pos);

	err = generic_segment_checks(iov, &nr_segs, &count, VERIFY_READ);
	if (err)
		return err;

	mutex_lock(&inode->i_mutex);
	vfs_check_frozen(inode->i_sb, SB_FREEZE_WRITE);

	/* We can write back this queue in page reclaim */
	current->backing_dev_info = mapping->backing_dev_info;

	err = generic_write_checks(file, &pos, &count, S_ISBLK(inode->i_mode));
	if (err)
		goto out;

	if (count == 0)
		goto out;

	err = file_remove_suid(file);
	if (err)
		goto out;

	file_update_time(file);

	iov_iter_init(&i, iov, nr_segs, count, 0);
	written = fuse_perform_write(file, mapping, &i, pos);
	if (written >= 0)
		iocb->ki_pos = pos + written;

out:
	current->backing_dev_info = NULL;
	mutex_unlock(&inode->i_mutex);

	return written ? written : err;
}