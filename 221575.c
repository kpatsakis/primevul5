static bool __io_file_supports_async(struct file *file, int rw)
{
	umode_t mode = file_inode(file)->i_mode;

	if (S_ISBLK(mode)) {
		if (IS_ENABLED(CONFIG_BLOCK) &&
		    io_bdev_nowait(I_BDEV(file->f_mapping->host)))
			return true;
		return false;
	}
	if (S_ISCHR(mode) || S_ISSOCK(mode))
		return true;
	if (S_ISREG(mode)) {
		if (IS_ENABLED(CONFIG_BLOCK) &&
		    io_bdev_nowait(file->f_inode->i_sb->s_bdev) &&
		    file->f_op != &io_uring_fops)
			return true;
		return false;
	}

	/* any ->read/write should understand O_NONBLOCK */
	if (file->f_flags & O_NONBLOCK)
		return true;

	if (!(file->f_mode & FMODE_NOWAIT))
		return false;

	if (rw == READ)
		return file->f_op->read_iter != NULL;

	return file->f_op->write_iter != NULL;
}